#!/usr/bin/env python3
"""
Génère un diagramme en barres à partir de la sortie de csParallelTask_test.
Usage:
  python plot_benchmark.py < sortie.txt
  python plot_benchmark.py sortie.txt
  csParallelTask_test.exe | python plot_benchmark.py
"""

import re
import sys
import os
import platform
import subprocess
from pathlib import Path

try:
    import matplotlib.pyplot as plt
    import matplotlib
    matplotlib.rcParams["font.size"] = 10
except ImportError:
    print("Installez matplotlib: pip install matplotlib")
    sys.exit(1)


def get_machine_info():
    """Récupère les caractéristiques de la machine."""
    info = {
        "system": platform.system(),
        "machine": platform.machine(),
        "processor": platform.processor() or "N/A",
        "cpu_count": None,
        "cpu_name": None,
    }
    try:
        info["cpu_count"] = str(os.cpu_count() or "?")
    except Exception:
        info["cpu_count"] = "?"

    # Nom du CPU (Windows: wmic, Linux: /proc/cpuinfo ou lscpu)
    if sys.platform == "win32":
        try:
            out = subprocess.run(
                ["wmic", "cpu", "get", "name", "/value"],
                capture_output=True, text=True, timeout=5
            )
            if out.returncode == 0 and out.stdout:
                for line in out.stdout.strip().splitlines():
                    if line.startswith("Name="):
                        info["cpu_name"] = line.split("=", 1)[1].strip()
                        break
        except Exception:
            pass
    else:
        try:
            with open("/proc/cpuinfo", "r", encoding="utf-8", errors="ignore") as f:
                for line in f:
                    if line.strip().startswith("model name"):
                        info["cpu_name"] = line.split(":", 1)[1].strip()
                        break
        except Exception:
            try:
                out = subprocess.run(["lscpu"], capture_output=True, text=True, timeout=5)
                if out.returncode == 0:
                    for line in out.stdout.splitlines():
                        if "Model name:" in line:
                            info["cpu_name"] = line.split(":", 1)[1].strip()
                            break
            except Exception:
                pass

    return info


def parse_output(text):
    """Parse la sortie texte et retourne (n_threads, N, benchmarks)."""
    n_threads = None
    N = None
    benchmarks = []  # list of {"name": str, "t_seq": int, "t_par": int|None, "speedup": float|None}

    re_header = re.compile(r"csParallelTask - (\d+) threads")
    re_n = re.compile(r"N = (\d+)")
    re_section = re.compile(r"^\s*\d+\.\s+(.+)$")
    re_t_seq = re.compile(r"Temps seq\s*:\s*(\d+)\s*us")
    re_t_par = re.compile(r"Temps par\s*:\s*(\d+)\s*us")
    re_speedup = re.compile(r"Speedup\s*:\s*([\d.]+)x")
    re_exec_time = re.compile(r"execute\([^)]+\)\s*:\s*(\d+)\s*us")

    lines = text.splitlines()
    i = 0
    current_name = None
    current_t_seq = None
    current_t_par = None
    current_speedup = None

    while i < len(lines):
        line = lines[i]
        m = re_header.search(line)
        if m:
            n_threads = int(m.group(1))
            i += 1
            continue
        m = re_n.search(line)
        if m:
            N = int(m.group(1))
            i += 1
            continue
        m = re_section.search(line)
        if m:
            if current_name is not None:
                benchmarks.append({
                    "name": current_name.strip(),
                    "t_seq": current_t_seq,
                    "t_par": current_t_par,
                    "speedup": current_speedup,
                })
            current_name = m.group(1).strip()
            current_t_seq = current_t_par = current_speedup = None
            i += 1
            continue
        m = re_t_seq.search(line)
        if m:
            current_t_seq = int(m.group(1))
            i += 1
            continue
        m = re_t_par.search(line)
        if m:
            current_t_par = int(m.group(1))
            i += 1
            continue
        m = re_speedup.search(line)
        if m:
            current_speedup = float(m.group(1))
            i += 1
            continue
        m = re_exec_time.search(line)
        if m:
            if current_name is not None:
                benchmarks.append({
                    "name": current_name.strip(),
                    "t_seq": current_t_seq,
                    "t_par": current_t_par,
                    "speedup": current_speedup,
                })
            current_name = "Execution par nom"
            current_t_seq = None
            current_t_par = int(m.group(1))
            current_speedup = None
            i += 1
            continue
        i += 1

    if current_name is not None:
        benchmarks.append({
            "name": current_name.strip(),
            "t_seq": current_t_seq,
            "t_par": current_t_par,
            "speedup": current_speedup,
        })

    return n_threads, N, benchmarks


def plot_bars(n_threads, N, benchmarks, machine_info, out_path):
    """Génère le diagramme en barres et le sauvegarde."""
    # Ne garder que les benchmarks avec au moins t_seq ou t_par
    data = [b for b in benchmarks if b["t_seq"] is not None or b["t_par"] is not None]
    if not data:
        print("Aucune donnée à afficher.")
        return

    labels = [b["name"][:25] + ("..." if len(b["name"]) > 25 else "") for b in data]
    x = range(len(labels))
    width = 0.35

    t_seq = [b["t_seq"] if b["t_seq"] is not None else 0 for b in data]
    t_par = [b["t_par"] if b["t_par"] is not None else 0 for b in data]

    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 10), height_ratios=[1.2, 1])

    # --- Graphique 1: Temps (barres groupées) ---
    bars1 = ax1.bar([i - width/2 for i in x], t_seq, width, label="Temps seq (us)", color="steelblue", alpha=0.9)
    bars2 = ax1.bar([i + width/2 for i in x], t_par, width, label="Temps par (us)", color="coral", alpha=0.9)
    ax1.set_ylabel("Temps (µs)")
    ax1.set_title("Temps d'exécution séquentiel vs parallèle")
    ax1.set_xticks(x)
    ax1.set_xticklabels(labels, rotation=25, ha="right")
    ax1.legend()
    ax1.grid(axis="y", linestyle="--", alpha=0.6)

    # --- Graphique 2: Speedup (barres) ---
    has_speedup = [b["speedup"] is not None for b in data]
    speedups = [b["speedup"] if b["speedup"] is not None else 0 for b in data]
    colors = ["green" if s else "lightgray" for s in speedups]
    ax2.bar(x, speedups, width=0.6, color=colors, alpha=0.85, edgecolor="black", linewidth=0.5)
    ax2.axhline(y=1, color="gray", linestyle="--", linewidth=1)
    ax2.set_ylabel("Speedup (x)")
    ax2.set_xlabel("Benchmark")
    ax2.set_title("Speedup (Temps seq / Temps par)")
    ax2.set_xticks(x)
    ax2.set_xticklabels(labels, rotation=25, ha="right")
    ax2.grid(axis="y", linestyle="--", alpha=0.6)

    # --- Infos machine en titre global ---
    cpu = machine_info.get("cpu_name") or machine_info.get("processor") or "CPU"
    n_cpu = machine_info.get("cpu_count", "?")
    fig.suptitle(
        f"csParallelTask — {n_threads} threads, N = {N:,}  |  {platform.node()}  |  {cpu}  ({n_cpu} logical cores)",
        fontsize=11,
        y=1.02,
    )

    plt.tight_layout()
    plt.savefig(out_path, dpi=150, bbox_inches="tight")
    plt.close()
    print(f"Figure sauvegardée : {out_path}")


def main():
    if len(sys.argv) > 1:
        with open(sys.argv[1], "r", encoding="utf-8", errors="replace") as f:
            text = f.read()
    else:
        text = sys.stdin.read()

    n_threads, N, benchmarks = parse_output(text)
    if n_threads is None:
        n_threads = "?"
    if N is None:
        N = "?"

    machine_info = get_machine_info()
    out_dir = Path(__file__).resolve().parent.parent
    out_path = out_dir / "csParallelTask_benchmark.png"
    plot_bars(n_threads, N, benchmarks, machine_info, out_path)


if __name__ == "__main__":
    main()
