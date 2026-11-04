# Używamy oficjalnego obrazu ROOT-a (Ubuntu 22.04 + ROOT 6.30)
FROM rootproject/root:6.30.04-ubuntu22.04

# Przełączamy się na root-a, żeby doinstalować pakiety
USER root
RUN apt-get update && apt-get install -y wget cmake g++ make tar && rm -rf /var/lib/apt/lists/*

# Pobieranie i kompilacja Pythia 8 (zajmuje ok. 2-3 minuty)
WORKDIR /opt
RUN wget https://pythia.org/download/pythia83/pythia8318.tgz && \
    tar -xvf pythia8318.tgz && \
    rm pythia8318.tgz && \
    cd pythia8318 && \
    ./configure && \
    make -j$(nproc)

# Ustawienie zmiennych środowiskowych, z których skorzysta Twój CMake i C++
ENV PYTHIA8DATA="/opt/pythia8318/share/Pythia8/xmldoc"
ENV PYTHIA_PATH="/opt/pythia8318"

# Skopiowanie Twojego projektu do kontenera
WORKDIR /workspace
COPY . /workspace

# Kompilacja Twojego kodu
WORKDIR /workspace/build
RUN cmake .. && make -j$(nproc)

# Domyślna komenda po uruchomieniu kontenera - odpala terminal
CMD ["/bin/bash"]