FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    curl \
    wget \
    git \
    python3 \
    python3-pip \
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

# Install Arduino IDE 2 (AppImage extracted to avoid FUSE requirement)
RUN curl -fsSL https://downloads.arduino.cc/arduino-ide/arduino-ide_latest_Linux_64bit.AppImage \
    -o /opt/arduino-ide.AppImage && \
    chmod +x /opt/arduino-ide.AppImage && \
    cd /opt && /opt/arduino-ide.AppImage --appimage-extract && \
    mv /opt/squashfs-root /opt/arduino-ide && \
    rm /opt/arduino-ide.AppImage && \
    ln -s /opt/arduino-ide/arduino-ide /usr/local/bin/arduino-ide

# Add udev rules for common Arduino/microcontroller boards
COPY udev/99-usb-serial.rules /etc/udev/rules.d/99-usb-serial.rules

# Build args for host UID/GID
ARG USER_UID=1000
ARG USER_GID=1000

RUN groupadd -g ${USER_GID} coder && \
    useradd -m -s /bin/bash -u ${USER_UID} -g ${USER_GID} -G sudo,dialout,plugdev coder && \
    echo "coder ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

USER coder
WORKDIR /home/coder

RUN pip3 install --user platformio
ENV PATH="/home/coder/.local/bin:$PATH"

RUN code --no-sandbox --install-extension platformio.platformio-ide

CMD ["sleep", "infinity"]