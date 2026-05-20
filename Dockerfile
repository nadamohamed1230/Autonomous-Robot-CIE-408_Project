# Use an official lightweight Python runtime optimized for ARM architectures
FROM python:3.11-slim

# Set system environment variables
ENV PYTHONDONTWRITEBYTECODE=1
ENV PYTHONUNBUFFERED=1

# Install essential system utilities required for compilation and serial interfaces
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

# Establish the working directory inside the container
WORKDIR /app

# Copy and install Python dependencies early to leverage Docker layer caching
COPY requirements.txt /app/
RUN pip install --no-cache-dir -r requirements.txt

# Copy the rest of your local project files into the container space
COPY . /app/

# The deployment entry point script
CMD ["python", "car_main_ai.py"]
# docker run -it --privileged --device=/dev/serial0:/dev/serial0 my_car_image