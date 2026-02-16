FROM python:3.11

# Install g++ and json library
RUN apt-get update && apt-get install -y \
    g++ \
    build-essential \
    nlohmann-json3-dev

WORKDIR /app

# Copy all project files
COPY . .

# Compile C++ analyzer
RUN g++ -std=c++17 test_main.cpp core.cpp -o main


# Install Flask
RUN pip install flask

# Expose port
EXPOSE 5000

# Run Flask app
CMD ["python", "smart-log-web/app.py"]
