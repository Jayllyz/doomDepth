FROM ubuntu:22.04 as base

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    bash \
    build-essential \
    cmake \
    make \
    sqlite3 \
    libsqlite3-dev \
    clang \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

CMD ["/bin/bash"]