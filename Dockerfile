FROM ubuntu:14.04
MAINTAINER Daniel Exner "dex+github@dragonslave.de"

# Tell debconf to run in non-interactive mode
ENV DEBIAN_FRONTEND noninteractive

# Make sure the repository information is up to date
RUN apt-get update

# We need ssh to access the docker container
RUN apt-get install -y openssh-server

# Install Moneypenny
ADD moneypenny-4.0_amd64.deb /moneypenny-4.0_amd64.deb
RUN dpkg -i /moneypenny-4.0_amd64.deb || true
RUN apt-get install -fy						# Automatically detect and install dependencies


# Create user "docker" and set empty password
RUN useradd -m -s /bin/bash docker
RUN echo "docker:docker" | chpasswd


# Create OpenSSH privilege separation directory, enable X11Forwarding
RUN mkdir -p /var/run/sshd
RUN echo X11Forwarding yes >> /etc/ssh/ssh_config

# Prepare ssh config folder so we can upload SSH public key later
RUN mkdir /home/docker/.ssh
RUN chown -R docker:docker /home/docker
RUN chown -R docker:docker /home/docker/.ssh

# Set locale (fix locale warnings)
RUN localedef -v -c -i en_US -f UTF-8 en_US.UTF-8 || true
RUN echo "Europe/Prague" > /etc/timezone

# Expose the SSH port
EXPOSE 22

# Start SSH
ENTRYPOINT ["/usr/sbin/sshd",  "-D"]