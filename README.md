# 📌 Portfolio - Ludivine Mooh

Welcome to my portfolio repository! This space serves as a collection of my research, projects, and ongoing cryptographic implementations. My primary focus is **post-quantum cryptography (PQC) software optimizations**.

---

## 📖 Research & Publications

### **🔹 Publications**
- **"ECC Accelerator using Faster Montgomery Ladder on FPGA Devices"** - *Busan Cyber Security Conference, 2024*
- **"Enhancing ML-KEM Performance using Modular Reduction as Macro"** - *MITA 2024*

### **🔹 Ongoing Research**
- **Optimizing ML-KEM (Kyber) for IoT Devices**: Improving memory efficiency and computational performance by replacing Keccak with Ascon-based primitives.
- **Security & Performance Evaluations**: Investigating IND-CCA2 security proofs, collision resistance, and implementation performance across various environments
- **Thesis** : A Lightweight ML-KEM for embedded devices (Iot, RISC V,) .

---

## 💻 Code Implementations
This repository contains some of my cryptographic implementations and performance testing scripts.

### **🔹 `code/` Folder - Cryptographic Implementations**

#### `ascon_mlkem.c`
- **Description:** Partial implementation of **Ascon-based ML-KEM**, replacing Keccak-based hashing with Ascon-XOF for improved memory efficiency.
- **Status:** 🚧 *Work in progress* – Ascon integration is still being optimized for key encapsulation and decapsulation.

#### `iot_memory_test.c`
- **Description:** Memory profiling and execution time analysis for ML-KEM in IoT environments.
- **Includes:** Static memory tracking, RAM usage estimation, and flash size monitoring.

📌 **Note:** These implementations are experimental and part of ongoing research. They are not intended for direct use but serve as a reference for my work.

---

 📬 **Contact**
 - For my full CV, see **cv_mooh.pdf** in this repository.
If you have any questions or are interested in discussing my work, feel free to reach out!
- **Email:** esther.ludivine@gmail.com
  

🚀 Thank you for visiting my portfolio!
