#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
import os
import sys

def main(csv_path="output/tree_metrics.csv"):
    # Garante que a saída vá para a pasta "output"
    out_dir = os.path.dirname(csv_path) or "output"
    os.makedirs(out_dir, exist_ok=True)

    # Lê o CSV
    df = pd.read_csv(csv_path)
    print(f"Arquivo '{csv_path}' carregado com {len(df)} segmentos.")

    # -----------------------------
    # 1️⃣ Histograma dos comprimentos
    # -----------------------------
    plt.figure(figsize=(8, 5))
    df["length"].hist(bins=30, color="#1f77b4", edgecolor="black")
    plt.title("Distribuição dos Comprimentos dos Segmentos")
    plt.xlabel("Comprimento (m)")
    plt.ylabel("Frequência")
    plt.grid(alpha=0.3)
    plt.tight_layout()
    plt.savefig(os.path.join(out_dir, "length_hist.png"), dpi=150)
    plt.close()

    # -----------------------------
    # 2️⃣ Boxplot dos raios por profundidade
    # -----------------------------
    plt.figure(figsize=(8, 5))
    df.boxplot(column="radius", by="depth", grid=False)
    plt.title("Distribuição dos Raios por Profundidade")
    plt.suptitle("")  # remove título automático do pandas
    plt.xlabel("Profundidade")
    plt.ylabel("Raio (m)")
    plt.tight_layout()
    plt.savefig(os.path.join(out_dir, "radius_by_depth.png"), dpi=150)
    plt.close()

    print(f"Gráficos salvos em: {out_dir}/length_hist.png e {out_dir}/radius_by_depth.png")

if __name__ == "__main__":
    csv = sys.argv[1] if len(sys.argv) > 1 else "output/tree_metrics.csv"
    main(csv)
