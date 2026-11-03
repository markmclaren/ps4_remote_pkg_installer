# syntax=docker/dockerfile:1

# ==============================================================================
# Stage 1: Fetch the open-source OpenOrbis PS4 Toolchain release
# ==============================================================================
FROM ubuntu:20.04 AS toolchain-fetcher

ENV DEBIAN_FRONTEND=noninteractive
ENV OO_PS4_TOOLCHAIN=/opt/openorbis
ARG OO_TOOLCHAIN_VERSION=v0.5.2

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      ca-certificates \
      curl \
      tar && \
    rm -rf /var/lib/apt/lists/*

SHELL ["/bin/bash", "-o", "pipefail", "-c"]
RUN mkdir -p "${OO_PS4_TOOLCHAIN}" && \
    curl -fsSL "https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain/releases/download/${OO_TOOLCHAIN_VERSION}/${OO_TOOLCHAIN_VERSION}.tar.gz" | \
    tar -xz -C "${OO_PS4_TOOLCHAIN}" && \
    mv "${OO_PS4_TOOLCHAIN}"/OpenOrbis/PS4Toolchain/* "${OO_PS4_TOOLCHAIN}/" && \
    rm -rf "${OO_PS4_TOOLCHAIN}/OpenOrbis"

# ==============================================================================
# Stage 2: OpenOrbis Builder Environment (Clang/LLD + LibOrbisPkg runtime deps)
# ==============================================================================
FROM ubuntu:20.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive
ENV OO_PS4_TOOLCHAIN=/opt/openorbis
ENV PATH="${OO_PS4_TOOLCHAIN}:${OO_PS4_TOOLCHAIN}/bin/linux:${PATH}"

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      build-essential \
      clang-12 \
      lld-12 \
      llvm-12 \
      libicu66 \
      libssl1.1 \
      ca-certificates && \
    ln -sf /usr/bin/clang-12 /usr/bin/clang && \
    ln -sf /usr/bin/clang++-12 /usr/bin/clang++ && \
    ln -sf /usr/bin/llvm-ar-12 /usr/bin/llvm-ar && \
    ln -sf /usr/bin/ld.lld-12 /usr/bin/ld.lld && \
    rm -rf /var/lib/apt/lists/*

COPY --from=toolchain-fetcher ${OO_PS4_TOOLCHAIN} ${OO_PS4_TOOLCHAIN}

WORKDIR /src

CMD ["make", "-f", "Makefile.openorbis"]

