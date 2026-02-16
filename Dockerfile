FROM python:3.11

RUN apt-get update && apt-get install -y \
    g++ \
    build-essential \
    nlohmann-json3-dev

WORKDIR /app

COPY . .

RUN rm -f main && g++ -std=c++17 test_main.cpp core.cpp -o main

RUN pip install flask

EXPOSE 5000

# IMPORTANT
WORKDIR /app/smart-log-web
CMD ["python", "app.py"]
