FROM rootproject/root:6.30.04-ubuntu22.04

# Changing to root to install packages
USER root
RUN apt-get update && apt-get install -y wget cmake g++ make tar git && rm -rf /var/lib/apt/lists/*

# Downloading and compiling
WORKDIR /opt
RUN git clone https://gitlab.com/Pythia8/releases.git --branch pythia8318 --single-branch pythia8318 && \
    cd pythia8318 && \
    ./configure && \
    make -j$(nproc)

# Setting up env variables
ENV PYTHIA8DATA="/opt/pythia8318/share/Pythia8/xmldoc"
ENV PYTHIA_PATH="/opt/pythia8318"

# Copying solution to container
WORKDIR /workspace
COPY . /workspace

# Compiling solution
WORKDIR /workspace/build
RUN cmake .. && make -j$(nproc)


CMD ["/bin/bash"]