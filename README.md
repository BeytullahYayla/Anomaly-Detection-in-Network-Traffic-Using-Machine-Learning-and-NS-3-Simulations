# 🚨 Flow-Based Network Anomaly Detection using NS-3 and Machine Learning

This project implements a network anomaly detection framework using the NS-3 network simulator and machine learning models. The system simulates realistic traffic in a five-subnet topology and detects cyberattacks based on flow-level statistics.

## 🔍 Overview

- **Simulator**: NS-3 (v3.36.1)
- **Attack Types**:  
  - UDP Flood  
  - Port Scan  
  - Brute-force TCP  
  - ICMP Ping Flood  
  - TCP Reset Flood
- **Detection Model**: Random Forest (supervised ML)
- **Metrics Used**: Throughput, Delay, Jitter, Packet Loss
