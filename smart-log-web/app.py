import subprocess
import os
import time
import json
from flask import Flask, render_template, request, send_file, jsonify

app = Flask(__name__)

# -----------------------------------------
# Proper Path Setup
# -----------------------------------------

# Folder where app.py exists (smart-log-web)
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

# uploads folder inside smart-log-web
UPLOAD_FOLDER = os.path.join(BASE_DIR, "uploads")

# main.exe is one level above smart-log-web
PROJECT_ROOT = os.path.abspath(os.path.join(BASE_DIR, ".."))
ANALYZER_PATH = os.path.join(PROJECT_ROOT, "main")

# Create uploads folder if not exists
if not os.path.exists(UPLOAD_FOLDER):
    os.makedirs(UPLOAD_FOLDER)

# -----------------------------------------
# HOME ROUTE
# -----------------------------------------

@app.route("/", methods=["GET", "POST"])
def home():

    if request.method == "POST":

        if "logfiles" not in request.files:
            return "No file part in request"

        files = request.files.getlist("logfiles")

        if not files or files[0].filename == "":
            return "No files selected"

        saved_paths = []

        # Save uploaded files
        for file in files:
            upload_path = os.path.join(UPLOAD_FOLDER, file.filename)
            file.save(upload_path)
            saved_paths.append(upload_path)

        # Merge files
        timestamp = str(int(time.time()))
        merged_filename = "merged_" + timestamp + ".txt"
        merged_path = os.path.join(UPLOAD_FOLDER, merged_filename)

        with open(merged_path, "w") as merged_file:
            for path in saved_paths:
                with open(path, "r") as f:
                    merged_file.write(f.read())
                    merged_file.write("\n")

        # Run analyzer
        output_filename = "output_" + timestamp + ".json"
        output_path = os.path.join(UPLOAD_FOLDER, output_filename)

        subprocess.run([ANALYZER_PATH, merged_path, output_path, "--json"])

        # Load JSON
        with open(output_path, "r") as f:
            data = json.load(f)

        errors_per_hour = data.get("errors_per_hour", {})
        log_levels = data.get("log_levels", {})
        categories = data.get("categories", {})

        return render_template(
    "results.html",
    errors=list(errors_per_hour.items()),
    levels=list(log_levels.items()),
    categories=list(categories.items()),
    alerts=[],
    filename=output_filename)


    return render_template("index.html")


# -----------------------------------------
# API ROUTE
# -----------------------------------------

@app.route("/api/analyze", methods=["POST"])
def api_analyze():

    if "logfiles" not in request.files:
        return jsonify({"error": "No file part in request"}), 400

    files = request.files.getlist("logfiles")

    if not files or files[0].filename == "":
        return jsonify({"error": "No files selected"}), 400

    saved_paths = []

    for file in files:
        upload_path = os.path.join(UPLOAD_FOLDER, file.filename)
        file.save(upload_path)
        saved_paths.append(upload_path)

    timestamp = str(int(time.time()))
    merged_filename = "merged_" + timestamp + ".txt"
    merged_path = os.path.join(UPLOAD_FOLDER, merged_filename)

    with open(merged_path, "w") as merged_file:
        for path in saved_paths:
            with open(path, "r") as f:
                merged_file.write(f.read())
                merged_file.write("\n")

    output_filename = "output_" + timestamp + ".json"
    output_path = os.path.join(UPLOAD_FOLDER, output_filename)

    subprocess.run([ANALYZER_PATH, merged_path, output_path, "--json"])

    with open(output_path, "r") as f:
        data = json.load(f)

    return jsonify(data), 200


# -----------------------------------------
# DOWNLOAD ROUTE
# -----------------------------------------

@app.route("/download/<filename>")
def download(filename):

    file_path = os.path.join(UPLOAD_FOLDER, filename)

    if not os.path.exists(file_path):
        return "File not found."

    return send_file(file_path, as_attachment=True)


# -----------------------------------------

if __name__ == "__main__":
    app.run(debug=True)
