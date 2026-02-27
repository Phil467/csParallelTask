/*
 * Programme de test csParallelTask : une dizaine d'exemples
 * comparant versions sequentielles et paralleles.
 */
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <numeric>
#include <algorithm>
#include <random>
#include "csPargs.h"
#include "csParallel.h"
#include "csPerfChecker.h"

using namespace std;
using namespace csParallelTask;

// ---- Constantes ----
size_t N = 10000000;
const int    UNIT = CSTIME_UNIT_MICROSECOND;

// ---- Kernels paralleles ----

static void kernel_sum(CSPARGS args) {
    double* data = args.getArgPtr<double>(0);
    double* global_sum = args.getArgPtr<double>(1);
    auto b = args.getBounds();
    size_t first = b.first;
    size_t last  = b.last;
    double sum = 0.0;
    for (size_t i = first; i < last; i++) sum += data[i];
    args.lockGuard();
    *global_sum += sum;
}

static void kernel_scale(CSPARGS args) {
    double* data = args.getArgPtr<double>(0);
    double* factor = args.getArgPtr<double>(1);
    auto b = args.getBounds();
    size_t first = b.first;
    size_t last  = b.last;
    double f = *factor;
    for (size_t i = first; i < last; i++) data[i] *= f;
}

static void kernel_min(CSPARGS args) {
    double* data = args.getArgPtr<double>(0);
    double* global_min = args.getArgPtr<double>(1);
    auto b = args.getBounds();
    size_t first = b.first;
    size_t last  = b.last;
    double m = data[first];
    for (size_t i = first + 1; i < last; i++)
        if (data[i] < m) m = data[i];
    args.lockGuard();
    if (m < *global_min) *global_min = m;
}

static void kernel_max(CSPARGS args) {
    double* data = args.getArgPtr<double>(0);
    double* global_max = args.getArgPtr<double>(1);
    auto b = args.getBounds();
    size_t first = b.first;
    size_t last  = b.last;
    double m = data[first];
    for (size_t i = first + 1; i < last; i++)
        if (data[i] > m) m = data[i];
    args.lockGuard();
    if (m > *global_max) *global_max = m;
}

static void kernel_dot(CSPARGS args) {
    double* a = args.getArgPtr<double>(0);
    double* b = args.getArgPtr<double>(1);
    double* global_dot = args.getArgPtr<double>(2);
    auto r = args.getBounds();
    size_t first = r.first;
    size_t last  = r.last;
    double sum = 0.0;
    for (size_t i = first; i < last; i++) sum += a[i] * b[i];
    args.lockGuard();
    *global_dot += sum;
}

static void kernel_fill(CSPARGS args) {
    double* data = args.getArgPtr<double>(0);
    double* value = args.getArgPtr<double>(1);
    auto b = args.getBounds();
    size_t first = b.first;
    size_t last  = b.last;
    double v = *value;
    for (size_t i = first; i < last; i++) data[i] = v;
}

static void kernel_sqsum(CSPARGS args) {
    double* data = args.getArgPtr<double>(0);
    double* global_sum = args.getArgPtr<double>(1);
    auto b = args.getBounds();
    size_t first = b.first;
    size_t last  = b.last;
    double sum = 0.0;
    for (size_t i = first; i < last; i++) { double x = data[i]; sum += x * x; }
    args.lockGuard();
    *global_sum += sum;
}

static void kernel_axpy(CSPARGS args) {
    double* x = args.getArgPtr<double>(0);
    double* y = args.getArgPtr<double>(1);
    double* alpha = args.getArgPtr<double>(2);
    auto b = args.getBounds();
    size_t first = b.first;
    size_t last  = b.last;
    double a = *alpha;
    for (size_t i = first; i < last; i++) y[i] = a * x[i] + y[i];
}

// ---- Versions sequentielles ----

static double seq_sum(const vector<double>& data) {
    return accumulate(data.begin(), data.end(), 0.0);
}

static void seq_scale(vector<double>& data, double factor) {
    for (auto& v : data) v *= factor;
}

static double seq_min(const vector<double>& data) {
    return *min_element(data.begin(), data.end());
}

static double seq_max(const vector<double>& data) {
    return *max_element(data.begin(), data.end());
}

static double seq_dot(const vector<double>& a, const vector<double>& b) {
    double s = 0.0;
    size_t size = a.size();
    for (size_t i = 0; i < size; i++) s += a[i] * b[i];
    return s;
}

static void seq_fill(vector<double>& data, double value) {
    fill(data.begin(), data.end(), value);
}

static double seq_norm2(const vector<double>& data) {
    double s = 0.0;
    for (double x : data) s += x * x;
    return sqrt(s);
}

static void seq_axpy(double alpha, const vector<double>& x, vector<double>& y) {
    size_t size = x.size();
    for (size_t i = 0; i < size; i++) y[i] = alpha * x[i] + y[i];
}

// ---- Utilitaire affichage ----

static void print_section(const char* title) {
    cout << "\n" << string(60, '=') << "\n  " << title << "\n" << string(60, '=') << "\n";
}

static void print_perf(const char* label_seq, size_t t_seq, const char* label_par, size_t t_par) {
    cout << "  " << label_seq << " : " << t_seq << " us\n";
    cout << "  " << label_par << " : " << t_par << " us\n";
    if (t_par > 0)
        cout << "  Speedup : " << fixed << setprecision(2) << (double)t_seq / (double)t_par << "x\n";
}

// ---- Main ----

int main() {
    cout << "csParallelTask - " << getMaxThreadNumber() << " threads\n  N = " << N << "\n";

    vector<double> data(N), data2(N);
    for (size_t i = 0; i < N; i++) {
        data[i] = sin(0.001 * i) + cos(0.002 * i);
        data2[i] = 1.0 / (1 + i % 100);
    }

    CSPERF_CHECKER perf(UNIT);
    size_t nThreads = getSafeThreadNumber(getMaxThreadNumber());

    // -------------------------------------------------------------------------
    print_section("1. Somme des elements");
    double sum_seq = 0.0, sum_par = 0.0;
    perf.start();
    sum_seq = seq_sum(data);
    perf.stop();
    size_t t_sum_seq = perf.getEllapsedTime();

    size_t id_sum = registerFunctionRegularEx(nThreads, N, (char*)"sum", kernel_sum,
        data.data(), &sum_par);
    perf.start();
    execute(id_sum);
    perf.stop();
    size_t t_sum_par = perf.getEllapsedTime();

    cout << "  sum (seq) = " << fixed << setprecision(6) << sum_seq << "\n";
    cout << "  sum (par) = " << sum_par << "\n";
    print_perf("Temps seq", t_sum_seq, "Temps par", t_sum_par);
    unregisterFunction(id_sum);

    // -------------------------------------------------------------------------
    print_section("2. Scale (in-place)");
    vector<double> data_scale = data;
    double factor = 2.5;

    perf.start();
    seq_scale(data_scale, factor);
    perf.stop();
    size_t t_scale_seq = perf.getEllapsedTime();

    vector<double> data_scale2 = data;
    double factor2 = 2.5;
    size_t id_scale = registerFunctionRegularEx(nThreads, N, (char*)"scale", kernel_scale,
        data_scale2.data(), &factor2);
    perf.start();
    execute(id_scale);
    perf.stop();
    size_t t_scale_par = perf.getEllapsedTime();

    cout << "  scale(2.5) - premiers: " << data_scale2[0] << " " << data_scale2[1] << "\n";
    print_perf("Temps seq", t_scale_seq, "Temps par", t_scale_par);
    unregisterFunction(id_scale);

    // -------------------------------------------------------------------------
    print_section("3. Minimum");
    double min_seq, min_par = 1e300;
    perf.start();
    min_seq = seq_min(data);
    perf.stop();
    size_t t_min_seq = perf.getEllapsedTime();

    size_t id_min = registerFunctionRegularEx(nThreads, N, (char*)"min", kernel_min,
        data.data(), &min_par);
    perf.start();
    execute(id_min);
    perf.stop();
    size_t t_min_par = perf.getEllapsedTime();

    cout << "  min (seq) = " << min_seq << "  min (par) = " << min_par << "\n";
    print_perf("Temps seq", t_min_seq, "Temps par", t_min_par);
    unregisterFunction(id_min);

    // -------------------------------------------------------------------------
    print_section("4. Maximum");
    double max_seq, max_par = -1e300;
    perf.start();
    max_seq = seq_max(data);
    perf.stop();
    size_t t_max_seq = perf.getEllapsedTime();

    size_t id_max = registerFunctionRegularEx(nThreads, N, (char*)"max", kernel_max,
        data.data(), &max_par);
    perf.start();
    execute(id_max);
    perf.stop();
    size_t t_max_par = perf.getEllapsedTime();

    cout << "  max (seq) = " << max_seq << "  max (par) = " << max_par << "\n";
    print_perf("Temps seq", t_max_seq, "Temps par", t_max_par);
    unregisterFunction(id_max);

    // -------------------------------------------------------------------------
    print_section("5. Produit scalaire");
    double dot_seq, dot_par = 0.0;
    perf.start();
    dot_seq = seq_dot(data, data2);
    perf.stop();
    size_t t_dot_seq = perf.getEllapsedTime();

    size_t id_dot = registerFunctionRegularEx(nThreads, N, (char*)"dot", kernel_dot,
        data.data(), data2.data(), &dot_par);
    perf.start();
    execute(id_dot);
    perf.stop();
    size_t t_dot_par = perf.getEllapsedTime();

    cout << "  dot (seq) = " << dot_seq << "  dot (par) = " << dot_par << "\n";
    print_perf("Temps seq", t_dot_seq, "Temps par", t_dot_par);
    unregisterFunction(id_dot);

    // -------------------------------------------------------------------------
 
    print_section("6. Remplissage (fill)");
    vector<double> data_fill(N), data_fill2(N);
    double fill_val = 3.14;

    perf.start();
    seq_fill(data_fill, fill_val);
    perf.stop();
    size_t t_fill_seq = perf.getEllapsedTime();

    size_t id_fill = registerFunctionRegularEx(nThreads, N, (char*)"fill", kernel_fill,
        data_fill2.data(), &fill_val);
    perf.start();
    execute(id_fill);
    perf.stop();
    size_t t_fill_par = perf.getEllapsedTime();

    cout << "  fill(3.14) - premiers: " << data_fill2[0] << " " << data_fill2[1] << "\n";
    print_perf("Temps seq", t_fill_seq, "Temps par", t_fill_par);
    unregisterFunction(id_fill);

    // -------------------------------------------------------------------------
    print_section("7. Norme L2 (sum of squares + sqrt)");
    double norm_seq, norm_par = 0.0;
    perf.start();
    norm_seq = seq_norm2(data);
    perf.stop();
    size_t t_norm_seq = perf.getEllapsedTime();

    size_t id_sq = registerFunctionRegularEx(nThreads, N, (char*)"sqsum", kernel_sqsum,
        data.data(), &norm_par);
    perf.start();
    execute(id_sq);
    perf.stop();
    size_t t_sq_par = perf.getEllapsedTime();
    norm_par = sqrt(norm_par);

    cout << "  norm (seq) = " << norm_seq << "  norm (par) = " << norm_par << "\n";
    print_perf("Temps seq", t_norm_seq, "Temps par", t_sq_par);
    unregisterFunction(id_sq);

    // -------------------------------------------------------------------------
    print_section("8. Execution par nom (execute par nom)");
    size_t id_by_name = registerFunctionRegularEx(nThreads, N, (char*)"scale_by_name", kernel_scale,
        data.data(), &factor);
    perf.start();
    execute("scale_by_name");
    perf.stop();
    size_t t_by_name = perf.getEllapsedTime();
    cout << "  execute(\"scale_by_name\") : " << t_by_name << " us\n";
    unregisterFunction(id_by_name);

    // -------------------------------------------------------------------------
    print_section("9. Resize + updateArg (meme fonction, autre taille)");
    size_t N2 = N / 2;
    vector<double> small(N2, 1.0);
    double sum_small = 0.0;
    size_t id_resize = registerFunctionRegularEx(nThreads, N2, (char*)"sum_resize", kernel_sum,
        small.data(), &sum_small);

    perf.start();
    execute(id_resize);
    perf.stop();
    size_t t1 = perf.getEllapsedTime();
    cout << "  sum(small, N/2) = " << sum_small << "  temps: " << t1 << " us\n";

    setBufferShapeRegular(id_resize, N);
    small.resize(N, 1.0);
    sum_small = 0.0;
    updateArg(id_resize, {0, 1}, {small.data(), &sum_small});
    perf.start();
    execute(id_resize);
    perf.stop();
    size_t t2 = perf.getEllapsedTime();
    cout << "  sum(small, N)   = " << sum_small << "  temps: " << t2 << " us (apres setBufferShapeRegular + updateArg)\n";
    unregisterFunction(id_resize);

    // -------------------------------------------------------------------------
    print_section("10. AXPY (y = alpha*x + y)");
    vector<double> x_axpy = data, y_axpy(N, 1.0);
    double alpha = 0.5;

    perf.start();
    seq_axpy(alpha, x_axpy, y_axpy);
    perf.stop();
    size_t t_axpy_seq = perf.getEllapsedTime();

    vector<double> y_axpy2(N, 1.0);
    double alpha2 = 0.5;
    size_t id_axpy = registerFunctionRegularEx(nThreads, N, (char*)"axpy", kernel_axpy,
        data.data(), y_axpy2.data(), &alpha2);
    perf.start();
    execute(id_axpy);
    perf.stop();
    size_t t_axpy_par = perf.getEllapsedTime();

    cout << "  axpy - y[0] (seq) = " << y_axpy[0] << "  (par) = " << y_axpy2[0] << "\n";
    print_perf("Temps seq", t_axpy_seq, "Temps par", t_axpy_par);

    // Nettoyage final
    unregisterAll();
    cout << "\n  unregisterAll() appele - toutes les taches desenregistrees.\n";
    cout << string(60, '=') << "\n";

#if defined(_WIN32)
    system("pause");
#endif

    return 0;
}
