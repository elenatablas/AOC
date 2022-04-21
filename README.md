# AOC - Arquitectura y Organización de Computadores

## Auditoría
Todos los documentos que forman este repositorio incluidos en el commit inicial pertenecen al profesorado de la asignatura Arquitectura y Organización de Computadores [21/22] del grado Ingeniería Informática de la Universidad de Murcia.

La solución de los ejercicios y memorias asociadas redactadas en \LaTeX corresponden a Elena Pérez González-Tablas.

## Configuración 

Por defecto, el contenedor tendrá acceso a los ficheros del directorio actual desde el que lo ejecutemos (y a todos sus subdirectorios). Por tanto, normalmente lo ejecutaremos desde un directorio donde estén presentes los ficheros de la práctica que se esté realizando. Por ejemplo, si hemos descargado los materiales de la práctica 1 en el directorio /home/nombre_usuario/datos/aoc/practicas/AOC-prac1-materiales, y tenemos el script en /home/nombre_usuario/datos/aoc/practicas/aoc-docker-image, los siguientes comandos nos permitirían compilar y ejecutar varios programas dentro del contenedor:
```
  $ cd /home/nombre_usuario/datos/aoc/practicas/AOC-prac1-materiales
  $ sudo /home/nombre_usuario/datos/aoc/practicas/aoc-docker-image/aoc-docker-image bash
  # cd peak-performance
  # make all
  # ./peak-freq-gcc
  # ./peak-flops-icc
 ```

Se aconseja definir un alias (o hacer un pequeño script o función de bash) para ejecutar el script aoc-docker-image desde cualquier directorio. Por ejemplo con un comando similar a «alias a="sudo /home/nombre_usuario/datos/aoc/practicas/aoc-docker-image"» cada vez que iniciemos una nueva terminal.

En mi caso:

  * Opción 1

      ```
      $ cd /home/elenatablas/docker-images/aoc-imagen
      $ sudo aoc-docker-image bash
      ```
  * Opción 2

      ```
      $ alias aoc="sudo /home/elenatablas/docker-images/aoc-imagen/aoc-docker-image"
      ```
