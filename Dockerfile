# Usa a imagem Debian como base
FROM debian:latest

# Atualiza os pacotes e instala dependências essenciais, incluindo wget para baixar a versão mais recente do CMake
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    wget \
    libsdl2-dev \
    libsdl2-ttf-dev \
    gdb \
    git \
    pkg-config

# Instala a versão mais recente do CMake
RUN wget https://github.com/Kitware/CMake/releases/download/v3.29.0/cmake-3.29.0-linux-x86_64.sh && \
    chmod +x cmake-3.29.0-linux-x86_64.sh && \
    ./cmake-3.29.0-linux-x86_64.sh --skip-license --prefix=/usr/local && \
    rm cmake-3.29.0-linux-x86_64.sh

# Define o diretório de trabalho no container
WORKDIR /app

# Copia o código fonte para o container
COPY . .

# Cria o diretório build para compilar
RUN mkdir -p build
WORKDIR /app/build

# Compila o projeto usando a versão atualizada do CMake
RUN cmake -DCMAKE_BUILD_TYPE=Debug .. && \
    cmake --build .

# Comando para rodar a aplicação
CMD ["./TF"]
