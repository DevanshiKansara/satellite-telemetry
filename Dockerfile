FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential cmake ninja-build libgtest-dev \
        python3 python3-pytest cppcheck \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /work
COPY . .

CMD ["./scripts/run_tests.sh"]
