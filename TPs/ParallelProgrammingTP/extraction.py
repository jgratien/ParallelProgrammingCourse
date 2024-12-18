import os
import re
import pandas as pd

# Chemin du répertoire contenant les fichiers logs
log_directory = "." 
output_csv = "resultats_logs_corriges.csv"

# Regex pour extraire les données nécessaires
norm_y_pattern = r"\|\|y\|\|=(\d+)"
norm_y2_pattern = r"\|\|y2\|\| = (\d+)"
mpispvm_pattern = r"MPISpMV:(\d+\.\d+e[+-]?\d+)"
spvm_pattern = r"SpMV:(\d+\.\d+e[+-]?\d+)"

# Extraction robuste pour NX et NPI depuis le nom du fichier
filename_pattern = r"log-NX(\d+)-NPI(\d+)"

# Liste pour stocker les résultats
results = []

# Parcours des fichiers dans le répertoire
for filename in os.listdir(log_directory):
    if filename.startswith("log-") and not filename.startswith("."):  # S'assurer que ce sont des fichiers log
        filepath = os.path.join(log_directory, filename)

        # Lecture du contenu du fichier
        with open(filepath, "r") as file:
            content = file.read()

        # Extraction des données depuis le nom du fichier
        filename_match = re.search(filename_pattern, filename)
        if filename_match:
            nx = int(filename_match.group(1))
            npi = int(filename_match.group(2))
        else:
            print(f"Nom de fichier mal formaté, impossible d'extraire NX et NPI : {filename}")
            nx, npi = None, None

        # Extraction des données depuis le contenu du fichier
        norm_y_match = re.search(norm_y_pattern, content)
        norm_y = int(norm_y_match.group(1)) if norm_y_match else None

        norm_y2_match = re.search(norm_y2_pattern, content)
        norm_y2 = int(norm_y2_match.group(1)) if norm_y2_match else None

        mpispvm_match = re.search(mpispvm_pattern, content)
        mpispvm_time = float(mpispvm_match.group(1)) if mpispvm_match else None

        spvm_match = re.search(spvm_pattern, content)
        spvm_time = float(spvm_match.group(1)) if spvm_match else None

        # Calcul des métriques supplémentaires si possible
        if mpispvm_time and spvm_time and npi:
            acceleration = spvm_time / mpispvm_time
            efficiency = acceleration / npi
        else:
            acceleration = None
            efficiency = None

        # Ajout des résultats sous forme de dictionnaire
        results.append({
            "Fichier": filename,
            "NX": nx,
            "NPI": npi,
            "Norme ||y||": norm_y,
            "Norme ||y2||": norm_y2,
            "MPISpMV Time": mpispvm_time,
            "SpMV Time": spvm_time,
            "Acceleration": acceleration,
            "Efficiency": efficiency
        })

# Création d'un DataFrame Pandas pour organiser les résultats
df = pd.DataFrame(results)

# Tri des résultats pour les rendre plus lisibles (par NX et NPI)
df = df.sort_values(by=["NX", "NPI"])

# Sauvegarde dans un fichier CSV
df.to_csv(output_csv, index=False)

print(f"Extraction terminée ! Les résultats sont enregistrés dans le fichier {output_csv}.")

