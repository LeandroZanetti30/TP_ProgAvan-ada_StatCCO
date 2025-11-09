#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os
import sys

def main(csv_path="output.csv"):
    # Pega o diretório do arquivo CSV (se não tiver diretório, usa o atual)
    csv_dir = os.path.dirname(csv_path) or "."
    csv_filename = os.path.basename(csv_path)
    
    # Cria o nome base para os arquivos PNG (sem extensão .csv)
    base_name = os.path.splitext(csv_filename)[0]

    # Lê o CSV
    df = pd.read_csv(csv_path)
    print(f"Arquivo '{csv_path}' carregado com {len(df)} segmentos.")
    print(f"Colunas disponíveis: {list(df.columns)}")
    
    # Verifica se temos a coluna de profundidade
    has_depth = 'depth' in df.columns
    has_terminal = 'is_terminal' in df.columns
    
    if not has_depth:
        print("AVISO: Coluna 'depth' não encontrada. Criando profundidade a partir da estrutura...")
        # Calcula profundidade baseada na hierarquia
        depth_map = {}
        segments_by_parent = df.groupby('parent_id')['child_id'].apply(list).to_dict()
        
        def calculate_depth(node_id):
            if node_id in depth_map:
                return depth_map[node_id]
            if node_id == 0:  # raiz
                depth_map[node_id] = 0
                return 0
            # Encontra o pai deste nó
            parent_row = df[df['child_id'] == node_id]
            if len(parent_row) == 0:
                depth_map[node_id] = 1
                return 1
            parent_id = parent_row.iloc[0]['parent_id']
            depth = calculate_depth(parent_id) + 1
            depth_map[node_id] = depth
            return depth
        
        # Calcula profundidade para todos os nós
        all_nodes = set(df['parent_id']).union(set(df['child_id']))
        for node in all_nodes:
            calculate_depth(node)
        
        # Adiciona profundidade ao DataFrame
        df['depth'] = df['child_id'].map(depth_map)

    # -----------------------------
    # 1️⃣ Histograma dos comprimentos
    # -----------------------------
    plt.figure(figsize=(10, 6))
    df["length"].hist(bins=30, color="#1f77b4", edgecolor="black", alpha=0.7)
    plt.title("Distribuição dos Comprimentos dos Segmentos")
    plt.xlabel("Comprimento (m)")
    plt.ylabel("Frequência")
    plt.grid(alpha=0.3)
    
    # Adiciona estatísticas no gráfico
    mean_len = df["length"].mean()
    median_len = df["length"].median()
    plt.axvline(mean_len, color='red', linestyle='--', linewidth=2, label=f'Média: {mean_len:.4f}m')
    plt.axvline(median_len, color='green', linestyle='--', linewidth=2, label=f'Mediana: {median_len:.4f}m')
    plt.legend()
    
    plt.tight_layout()
    plt.savefig(os.path.join(csv_dir, f"{base_name}_length_hist.png"), dpi=150)
    plt.close()

    # -----------------------------
    # 2️⃣ Boxplot dos raios por profundidade
    # -----------------------------
    if has_depth or 'depth' in df.columns:
        plt.figure(figsize=(10, 6))
        
        # Agrupa por profundidade e cria boxplot
        depth_groups = df.groupby('depth')['radius'].apply(list)
        depths = sorted(depth_groups.index)
        
        plt.boxplot([depth_groups[d] for d in depths], labels=depths)
        plt.title("Distribuição dos Raios por Profundidade")
        plt.xlabel("Profundidade")
        plt.ylabel("Raio (m)")
        plt.grid(alpha=0.3)
        plt.tight_layout()
        plt.savefig(os.path.join(csv_dir, f"{base_name}_radius_by_depth.png"), dpi=150)
        plt.close()

    # -----------------------------
    # 3️⃣ Gráfico de raio vs comprimento
    # -----------------------------
    plt.figure(figsize=(10, 6))
    plt.scatter(df["radius"], df["length"], alpha=0.6, color='purple')
    plt.title("Relação entre Raio e Comprimento dos Segmentos")
    plt.xlabel("Raio (m)")
    plt.ylabel("Comprimento (m)")
    plt.grid(alpha=0.3)
    
    # Adiciona linha de tendência
    if len(df) > 1:
        z = np.polyfit(df["radius"], df["length"], 1)
        p = np.poly1d(z)
        plt.plot(df["radius"], p(df["radius"]), "r--", alpha=0.8, linewidth=2)
    
    plt.tight_layout()
    plt.savefig(os.path.join(csv_dir, f"{base_name}_radius_vs_length.png"), dpi=150)
    plt.close()

    # -----------------------------
    # 4️⃣ Distribuição de fluxos
    # -----------------------------
    plt.figure(figsize=(10, 6))
    df["flow"].hist(bins=30, color="#ff7f0e", edgecolor="black", alpha=0.7)
    plt.title("Distribuição dos Fluxos nos Segmentos")
    plt.xlabel("Fluxo (m³/s)")
    plt.ylabel("Frequência")
    plt.grid(alpha=0.3)
    
    mean_flow = df["flow"].mean()
    plt.axvline(mean_flow, color='red', linestyle='--', linewidth=2, label=f'Média: {mean_flow:.2e}m³/s')
    plt.legend()
    
    plt.tight_layout()
    plt.savefig(os.path.join(csv_dir, f"{base_name}_flow_hist.png"), dpi=150)
    plt.close()

    # -----------------------------
    # 5️⃣ Estatísticas resumidas
    # -----------------------------
    print("\n" + "="*50)
    print("ESTATÍSTICAS DA ÁRVORE VASCULAR")
    print("="*50)
    
    print(f"Total de segmentos: {len(df)}")
    print(f"Total de nós únicos: {len(set(df['parent_id']).union(set(df['child_id'])))}")
    
    if has_terminal:
        n_terminals = df['is_terminal'].sum() if df['is_terminal'].dtype == bool else len(df[df['is_terminal'] == 1])
        print(f"Terminais (nós azuis): {n_terminals}")
    
    if has_depth or 'depth' in df.columns:
        max_depth = df['depth'].max()
        print(f"Profundidade máxima: {max_depth}")
    
    print(f"\nCOMPRIMENTOS:")
    print(f"  Média: {df['length'].mean():.6f} m")
    print(f"  Mediana: {df['length'].median():.6f} m")
    print(f"  Mínimo: {df['length'].min():.6f} m")
    print(f"  Máximo: {df['length'].max():.6f} m")
    
    print(f"\nRAIOS:")
    print(f"  Média: {df['radius'].mean():.6f} m")
    print(f"  Mediana: {df['radius'].median():.6f} m")
    print(f"  Mínimo: {df['radius'].min():.6f} m")
    print(f"  Máximo: {df['radius'].max():.6f} m")
    
    print(f"\nFLUXOS:")
    print(f"  Média: {df['flow'].mean():.2e} m³/s")
    print(f"  Mediana: {df['flow'].median():.2e} m³/s")
    print(f"  Mínimo: {df['flow'].min():.2e} m³/s")
    print(f"  Máximo: {df['flow'].max():.2e} m³/s")
    
    if 'pressure_in' in df.columns and 'pressure_out' in df.columns:
        avg_pressure_drop = (df['pressure_in'] - df['pressure_out']).mean()
        print(f"\nQueda de pressão média: {avg_pressure_drop:.2f} Pa")

    print(f"\nGráficos salvos em: {csv_dir}/")
    print(f"Arquivos gerados:")
    print(f"  - {base_name}_length_hist.png")
    if has_depth or 'depth' in df.columns:
        print(f"  - {base_name}_radius_by_depth.png")
    print(f"  - {base_name}_radius_vs_length.png")
    print(f"  - {base_name}_flow_hist.png")
    print("="*50)

if __name__ == "__main__":
    csv = sys.argv[1] if len(sys.argv) > 1 else "output.csv"
    main(csv)