# Get the base Ubuntu image from Docker Hub
FROM ubuntu:20.04

# Install GCC and dependencies
RUN apt-get -y update \
    && DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt-get -y install tzdata \
    && apt-get install -y git make cmake gcc g++

# Start building
COPY . /usr/src/rle_string
WORKDIR /usr/src/rle_string/build
RUN cmake .. \
    && make -j

# Get binaries
WORKDIR /rle_string/bin
RUN cp \
    /usr/src/rle_string/build/rleinfo \
    .
ENV PATH /rle_string/bin:$PATH