FROM alpine:3.18 as base

RUN apk update && \
    apk add --no-cache bash git openssh build-base gcc cmake make sqlite sqlite-dev clang && \
    rm -rf /var/cache/apk/*

WORKDIR /app

CMD ["/bin/bash"]
