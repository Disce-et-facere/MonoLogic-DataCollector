# Use an official Node runtime as a parent image
FROM node:latest

# Set the working directory
WORKDIR /app

# Install Quasar CLI globally
RUN npm install -g @quasar/cli@latest

# Copy package.json and install dependencies
COPY package*.json ./

# Install dependencies
RUN npm install

# # Add the rest of the code
COPY . .

# Make port available
EXPOSE 9000

# Serve the app
CMD ["quasar", "dev"]