print("CORRECT APP FILE LOADED")

import subprocess
import os
import time
import json
from flask import Flask, render_template, request, send_file, jsonify

app = Flask(
    __name__,
    template_folder="templates",
    static_folder="static"
)


# -----------------------------------------
# Proper Path Setup
# -----------------------------------------

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
UPLOAD_FOLDER = os.path.join(BASE_DIR, "uploads")
PROJECT_ROOT = os.path.abspath(os.path.join(BASE_DIR, ".."))
ANALYZER_PATH = os.path.join(PROJECT_ROOT, "main")

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
        merged_filename = f"merged_{timestamp}.txt"
        merged_path = os.path.join(UPLOAD_FOLDER, merged_filename)

        with open(merged_path, "w") as merged_file:
            for path in saved_paths:
                with open(path, "r") as f:
                    merged_file.write(f.read())
                    merged_file.write("\n")

        # Prepare output path
        output_filename = f"output_{timestamp}.json"
        output_path = os.path.join(UPLOAD_FOLDER, output_filename)


        


        # Run analyzer
        result = subprocess.run(
            [ANALYZER_PATH, merged_path, output_path, "--json"],
            capture_output=True,
            text=True
        )

        if result.returncode != 0:
            return f"<h3>Analyzer Failed</h3><pre>{result.stderr}</pre>"

        if not os.path.exists(output_path):
            return "<h3>Output JSON file was not created.</h3>"

        try:
            with open(output_path, "r") as f:
                data = json.load(f)
        except Exception as e:
            return f"<h3>JSON Error</h3><pre>{str(e)}</pre>"

        # -----------------------------------------
        # FORCE NORMALIZATION (CRITICAL FIX)
        # -----------------------------------------

        errors_per_hour = data.get("errors_per_hour", [])
        log_levels = data.get("log_levels", {})
        categories = data.get("categories", {})

        # Errors may already be list from C++
        if isinstance(errors_per_hour, dict):
            errors = [[k, v] for k, v in errors_per_hour.items()]
        else:
            errors = errors_per_hour

        # ALWAYS convert dict → list of [key,value]
        levels = [[k, v] for k, v in log_levels.items()]
        categories_list = [[k, v] for k, v in categories.items()]
        alerts=data.get("alerts",[])
        skipped = data.get("skipped_lines", 0)


        return render_template(
            "results.html",
            errors=errors,
            levels=levels,
            categories=categories_list,
            alerts=alerts,
            skipped=skipped,
            filename=output_filename
        )

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
    merged_filename = f"merged_{timestamp}.txt"
    merged_path = os.path.join(UPLOAD_FOLDER, merged_filename)

    with open(merged_path, "w") as merged_file:
        for path in saved_paths:
            with open(path, "r") as f:
                merged_file.write(f.read())
                merged_file.write("\n")

    output_filename = f"output_{timestamp}.json"
    output_path = os.path.join(UPLOAD_FOLDER, output_filename)

    result = subprocess.run(
        [ANALYZER_PATH, merged_path, output_path, "--json"],
        capture_output=True,
        text=True
    )

    if result.returncode != 0:
        return jsonify({"error": result.stderr}), 500

    if not os.path.exists(output_path):
        return jsonify({"error": "Output file not created"}), 500

    try:
        with open(output_path, "r") as f:
            data = json.load(f)
            print("DEBUG JSON:", data)

    except Exception as e:
        return jsonify({"error": str(e)}), 500

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
    port = int(os.environ.get("PORT", 5000))
    app.run(host="0.0.0.0", port=port)

