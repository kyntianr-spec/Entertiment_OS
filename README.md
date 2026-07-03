# Entertiment OS

Kompletny system operacyjny napisany od zera w C i Assembly dla procesorów x86/x86-64.

## 🎯 Projekt

**Entertiment OS** to edukacyjny projekt pełnoprawnego systemu operacyjnego z:
- Bootloader (Legacy BIOS + UEFI)
- Kernel z obsługą multitaskingu
- System plików
- Sterowniki
- Shell CLI
- Obsługa x32, x64 i x86

## 📋 Struktura projektu

```
Entertiment_OS/
├── bootloader/          # Bootloader (NASM Assembly)
├── kernel/              # Kernel OS (C + Assembly)
├── drivers/             # Sterowniki urządzeń
├── filesystem/          # System plików
├── shell/               # Interpreter poleceń
├── libs/                # Biblioteki systemu
├── tools/               # Narzędzia (Media Creation Tool, etc.)
├── docs/                # Dokumentacja
├── build/               # Pliki build (Makefile, skrypty)
└── tests/               # Testy i emulatacja (QEMU)
```

## 🔧 Architektura

### Wspierane platformy:
- **x86-64** (64-bit) - główna architektura
- **x86** (32-bit) - wsparcie opcjonalne
- **x32** (32-bit ABI na x86-64) - wsparcie opcjonalne

### Bootowanie:
- **Legacy BIOS** - standardowy bootloader MBR
- **UEFI** - nowoczesny bootloader EFI (wsparcie do dodania)

### Komponenty

#### 1. Bootloader
- Przełączenie z real mode na protected/long mode
- Inicjalizacja GDT, IDT
- Załadowanie kernela do pamięci
- Włączenie paging'u

#### 2. Kernel
- Process management
- Memory management (paging, virtual memory)
- Interrupt handling (IDT, PIC/APIC)
- IPC (Inter-Process Communication)
- Scheduling (round-robin, priority-based)

#### 3. System plików
- Prosty FAT12/FAT16 lub własny format
- Obsługa katalogów
- Operacje na plikach (read/write/delete)

#### 4. Sterowniki
- ATA/SATA (dysk)
- Klawiatura
- Ekran (VGA/VESA)
- Sieć (opcjonalnie)

#### 5. Shell
- Interpreter poleceń
- Obsługa procesów
- Redirecty I/O

## 🚀 Jak zacząć

### Wymagania
- NASM (assembler)
- GCC (kompilator C)
- Make
- QEMU (emulator do testów)

### Budowanie
```bash
cd Entertiment_OS
make build          # Kompilacja całego projektu
make iso            # Tworzenie ISO
make run            # Uruchomienie w QEMU
make clean          # Czyszczenie
```

### Testowanie
```bash
make test           # Uruchamianie testów w QEMU
make debug          # Debug mode z GDB
```

## 📚 Dokumentacja

- [Bootloader](docs/bootloader.md)
- [Kernel Architecture](docs/kernel.md)
- [Memory Management](docs/memory.md)
- [Process Management](docs/processes.md)
- [Filesystem](docs/filesystem.md)
- [Drivers](docs/drivers.md)

## 🛠️ Media Creation Tool

Narzędzie do tworzenia bootowalnych nośników z Entertiment OS:
- Wsparcie dla x32, x64, x86
- BIOS i UEFI
- Tworzenie USB bootable
- Zapis na DVD/CD
- Weryfikacja sum kontrolnych
- Graficzny interfejs użytkownika (Qt5/6)

Więcej: [Media Creation Tool](tools/media_creation_tool/README.md)

## 📝 Status

- [x] Struktura projektu
- [ ] Bootloader (BIOS x86-64)
- [ ] Kernel (podstawy)
- [ ] System plików
- [ ] Process manager
- [ ] Sterowniki
- [ ] Shell
- [ ] Media Creation Tool
- [ ] Dokumentacja

## 👨‍💻 Autor

**kyntianr-spec** - Entertiment OS Project

## 📄 Licencja

MIT License

---

**Zbudujmy coś niesamowitego!** 🚀
