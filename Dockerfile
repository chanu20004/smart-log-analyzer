FROM python:3.11

RUN apt-get update && apt-get install -y \
    g++ \
    build-essential \
    nlohmann-json3-dev

WORKDIR /app

# Copy entire project
COPY . .

# Compile C++
RUN rm -f main && g++ -std=c++17 test_main.cpp core.cpp -o main

# Install Flask
RUN pip install flask

EXPOSE 5000

# IMPORTANT CHANGE HERE
CMD ["python", "smart-log-web/app.py"]
