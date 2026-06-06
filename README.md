# kmonitor-so2
# KMonitor — Grupo 9

Módulo Cargable del Kernel Linux (LKM) desarrollado en C que recolecta métricas críticas del sistema directamente desde las estructuras internas del kernel y las expone al usuario a través de un archivo en `/proc/kmonitor_grupo9`.

---

## Integrantes del Grupo 9

| Nombre | Rol |
|---|---|
| David | Desarrollo del módulo kernel y demostración |
| Derick | Desarrollo del código fuente |
| Dany | Documentación y coordinación |
| Marlon | Documentación de estructuras y video |

---

##  Requisitos

- Sistema operativo: Ubuntu 20.04 o superior
- Kernel: Linux 5.x o superior
- Herramientas: `git`, `gcc`, `make`, `linux-headers`

---

## Instalación y uso

### 1. Instalar Git (si no lo tienes)
```bash
sudo apt install git -y
```

### 2. Clonar el repositorio
```bash
cd ~
git clone https://github.com/tahual/kmonitor-so2.git
cd kmonitor-so2
```

### 3. Instalar herramientas de compilación
```bash
sudo apt-get install -y build-essential linux-headers-$(uname -r)
```

### 4. Compilar el módulo
```bash
cd src
make
```

### 5. Cargar el módulo
```bash
sudo insmod kmonitor_memoria.ko
```

### 6. Leer las métricas
```bash
cat /proc/kmonitor_grupo9
```

### 7. Descargar el módulo
```bash
sudo rmmod kmonitor_memoria
```

### 8. Verificar en el ring buffer del kernel
```bash
sudo dmesg | tail -5
```

---

## ¿Qué muestra el módulo?

Al ejecutar `cat /proc/kmonitor_grupo9` se obtiene:

```
=== MEMORIA RAM ===
Total:      3914 MB
Libre:       254 MB
Usada:      3660 MB
Uso:          93%

=== PROCESOS ACTIVOS ===
PID      ESTADO       NOMBRE
1        Sleeping     systemd
2        Sleeping     kthreadd
536      Running      kworker
5089     Sleeping     bash
...
```

---

## Documentación

- [Arquitectura de la solución](docs/arquitectura.md)
- [Estructuras del kernel utilizadas](docs/estructuras.md)
- [Tecnologias utilizadas](docs/tecnologias.md) 

---

## Información académica

| Campo | Detalle |
|---|---|
| Universidad | Universidad Mariano Gálvez de Guatemala |
| Facultad | Ingeniería en Sistemas de Información |
| Curso | Sistemas Operativos II |
| Proyecto | K-Monitor |
| Año | 2026 |

---

## Advertencia

Este módulo corre en **espacio de kernel**. Se recomienda ejecutarlo
siempre dentro de una máquina virtual para evitar daños al sistema
operativo principal.

---
