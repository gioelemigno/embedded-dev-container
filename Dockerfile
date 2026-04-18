FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

USER root

RUN apt-get update && apt-get install -y \
    curl \
    wget \
    git \
    python3 \
    python3-pip \
    python3-venv \
    sudo \
    locales \
    ca-certificates \
    gnupg \
    libx11-xcb1 \
    libxcb-dri3-0 \
    libdrm2 \
    libgbm1 \
    libglib2.0-0 \
    libnspr4 \
    libnss3 \
    libxcomposite1 \
    libxdamage1 \
    libxfixes3 \
    libxrandr2 \
    libxtst6 \
    libatspi2.0-0 \
    libsecret-1-0 \
    libkrb5-3 \
    libxss1 \
    libxkbfile1 \
    libdbus-1-3 \
    libexpat1 \
    fonts-dejavu-core \
    libfuse2 \
    unzip \
    # udev for hot-plug support
    udev \
    && rm -rf /var/lib/apt/lists/*

# Set locale
RUN locale-gen en_US.UTF-8
ENV LANG=en_US.UTF-8 \
    LANGUAGE=en_US:en \
    LC_ALL=en_US.UTF-8

# Install VS Code
RUN curl -fsSL https://packages.microsoft.com/keys/microsoft.asc | gpg --dearmor -o /usr/share/keyrings/microsoft.gpg && \
    echo "deb [arch=amd64 signed-by=/usr/share/keyrings/microsoft.gpg] https://packages.microsoft.com/repos/code stable main" \
    > /etc/apt/sources.list.d/vscode.list && \
    apt-get update && apt-get install -y code && \
    rm -rf /var/lib/apt/lists/*


ARG ARDUINO_IDE_VERSION=2.3.4
RUN wget https://downloads.arduino.cc/arduino-ide/arduino-ide_${ARDUINO_IDE_VERSION}_Linux_64bit.zip
RUN (mkdir /usr/local/share/arduino-${ARDUINO_IDE_VERSION} && \
     unzip -d /usr/local/share/arduino-${ARDUINO_IDE_VERSION} arduino-ide_${ARDUINO_IDE_VERSION}_Linux_64bit.zip && \
     ln -s /usr/local/share/arduino-${ARDUINO_IDE_VERSION} /usr/local/share/arduino && \
     ln -s /usr/local/share/arduino-${ARDUINO_IDE_VERSION}/arduino-ide /usr/local/bin/arduino-ide && \
     chmod 04755 /usr/local/share/arduino-${ARDUINO_IDE_VERSION}/chrome-sandbox)

# Add udev rules for common Arduino/microcontroller boards
COPY udev/99-usb-serial.rules /etc/udev/rules.d/99-usb-serial.rules

# Build args for host UID/GID
ARG USER_UID=err
ARG USER_GID=err

RUN groupadd -g ${USER_GID} coder && \
    useradd -m -s /bin/bash -u ${USER_UID} -g ${USER_GID} -G sudo,dialout,plugdev coder && \
    echo "coder ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

RUN mkdir -p /home/coder/.platformio && \
    chown -R coder:coder /home/coder/.platformio && \
    mkdir -p /home/coder/.arduino15 && \
    chown -R coder:coder /home/coder/.arduino15

# RUN chown -R coder:coder /opt/arduino-ide

USER coder
WORKDIR /home/coder

# RUN pip3 install --user platformio
ENV PATH="/home/coder/.local/bin:$PATH"
ENV PATH="/usr/local/bin:${PATH}"

RUN curl -fsSL -o get-platformio.py https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py && \
    python3 get-platformio.py

RUN code --no-sandbox --install-extension platformio.platformio-ide

WORKDIR /home/coder/workdir

CMD ["sleep", "infinity"]