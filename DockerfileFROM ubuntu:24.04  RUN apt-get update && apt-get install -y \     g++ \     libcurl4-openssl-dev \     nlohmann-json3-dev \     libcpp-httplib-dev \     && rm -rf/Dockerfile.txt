FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    g++ \
    libcurl4-openssl-dev \
    nlohmann-json3-dev \
    libcpp-httplib-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY main.cpp .
COPY website ./website

RUN g++ -std=c++17 main.cpp -o futurefunction -lcurl -pthread

CMD ["./futurefunction"]