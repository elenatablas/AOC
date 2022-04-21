* Entorno de trabajo para la prácticas de AOC

Las prácticas de la asignatura de Arquitectura y Organización de Computadores están pensadas para realizarse con unos compiladores concretos de C++. En concreto, vamos a utilizar compiladores de C++ modernos capaces de optimizar eficazmente programas para su ejecución en los procesadores actuales utilizando la versión actual del estándar del lenguaje y de OpenMP.

Es importante utilizar las versiones indicadas de los compiladores a la hora de realizar las prácticas para obtener resultados correctos y evitar problemas de compatibilidad. Aunque normalmente se podrá utilizar versiones más nuevas que las indicadas en este documento, los resultados obtenidos en ese caso pueden ser inesperados.

A la hora de la explicación y la realización de las prácticas se supondrá siempre un entorno Linux actualizado (por ejemplo: una versión actual de Fedora, Debian, Ubuntu o cualquier distribución similar). Es posible utilizar otros sistemas operativos para realizar las prácticas, aunque en ese caso el alumno será responsable de adaptar las explicaciones y materiales proporcionados como sea pertinente y solucionar los problemas de compatibilidad que pudieran surgir. Asimismo, se supondrá siempre que el ordenador utilizado soporta el ISA x86-64. No se aconseja el uso de máquinas virtuales (como QEMU, KVM, VirtualBox o similares) para la realización de las prácticas.

Todos los compiladores utilizados (y casi todo el software adicional necesario) están disponibles para los sistemas operativos más populares, incluyendo Linux, Windows y Mac OS X. Cualquier alumno puede instalarlos en su ordenador de trabajo siguiendo las instrucciones disponibles en las respetivas páginas WEB de cada compilador. Sin embargo, la instalación correcta de estos programas no es trivial y puede generar problemas, en especial si se quiere asegurar el uso de versiones correctas y evitar interferencias entre los tres compiladores y otras versiones de los mismos que también puedan estar presentes previamente en el sistema.

Usaremos principalmente el GCC versión 11.0.1 y el ICC versión 2021.3.0, y además mostraremos en algunos casos los resultados obtenidos también por Clang 12.0.0:

 - GCC (https://gcc.gnu.org/) es el compilador de GNU, que es capaz de generar código optimizado para prácticamente cualquier ISA y que esá incluido en todas las distribuciones de Linux. Sin embargo, la versión incluida puede no ser la adecuada para la realización de las prácticas. 

 - ICC es el compilador de Intel, el cual se puede descargar de la página web de la compañía (https://software.intel.com/content/www/us/en/develop/tools/oneapi/all-toolkits.html). La instalación de este compilador puede interferir con la de Clang, y además requiere de la presencia en el sistema de una librería estándar de C++ para compilar correctamente los programas de las prácticas normalmente provista por GCC (o MSVC en Windows). Es decir: necesita que GCC esté instalado previamente, y además actualmente necesita que la versión de la librería de C++ no sea demasiado reciente para poder usarla. Usaremos la versión «clásica» de ICC, aunque esta versión está a punto de dejar de ser soportada por Intel, que va a basar las nuevas versiones de su compilador en Clang. 

 - Clang es el compilador de C y C++ basado en LLVM (https://llvm.org/). Este compilador se orginó en la Universidad de Illinois y es actualmente muy usado. Es el compilador principal utilizado por Apple y, como se ha mencionado, Intel ha comenzado a usarlo como base para las nuevas versiones de su compilador.

Los tres compiladores soportan el estándar actual de C++ (C++20) y, como ya se ha mencionado, están disponibles en la mayoría de los sistemas operativos. Todos ellos son capaces de compilar más lenguajes además de C++ (notablemente C y Fortran, pero también Rust o Go).

* Uso del script aoc-docker-image

Aunque se ha mencionado que es posible instalar los compiladores y casi todo el resto del software necesario para las prácticas en casi cualquier sistema operativo, el proceso no es trivial y explicarlo queda fuera de los objetivos de las prácticas.

Para facilitar el acceso a las versiones adecuadas de los compiladores y algunos otros programas que se utilizarán durante las prácticas, se ha preparado un script de Bash que genera una imagen Docker que se puede utilizar, si se desea, para realizar las prácticas. El script, llamado aoc-docker-image, automatiza la creación de la imagen y su posterior ejecución de forma correcta.

El script requiere, lógicamente, tener instalado previamente Docker. Para ello, es suficiente con seguir las instrucciones disponibles en https://docs.docker.com/engine/install/ para el sistema operativo que se vaya a utilizar. En el caso de Linux, obsérvese que el procedimiento es diferente según la distribución utilizada. El script también funciona con moby-engine, que es la versión upstream de Docker (por lo que en distribuciones que lo incluyan es posible instalarlo directamente. Por ejemplo en Fedora es suficiente con «dnf install docker», sin necesidad de usar el repositorio docker-ce).

Una vez que esté instalado Docker y se haya comprobado que funciona (usando por ejemplo el comando «docker run -it hello-world»), se puede usar el script aoc-docker-image.

** Uso de aoc-docker-image en Linux

Si ejecutamos el script sin ningún argumento, nos mostrará un resumen de esta documentación. Normalmente usaremos siempre al menos un argumento, que será el comando que se quiera ejecutar dentro del contenedor.

Normalmente el uso de Docker en Linux requiere de privilegios de root, por lo que se debe usar sudo para ejecutar el script. Alternativamente, es posible otorgar permisos para usar Docker a cualquier usuario añadiéndolo al grupo correspondiente (por ejemplo, con un comando similar a «usermod nombre_usuario -a -G docker»).

La primera vez que ejecutemos el script, debemos usar el parámetro --build para generar la imagen. Esta operación descargará todo el software necesario y es necesario realizarla solo una vez. Por ejemplo, si estamos en el directorio del script, podemos escribir «sudo ./aoc-docker-image --build». Este proceso puede tardar bastante tiempo (según sea la velocidad de la conexión a Internet y del ordenador).

Una vez construida la imagen, podemos ejecutarla mediante el script. El script espera como argumentos el comando a ejecutar dentro del contenedor. El comando puede ser «bash», lo cual nos permitirá crear un shell en el contenedor desde el que podemos ejecutar varios comandos.

Por defecto, el contenedor tendrá acceso a los ficheros del directorio actual desde el que lo ejecutemos (y a todos sus subdirectorios). Por tanto, normalmente lo ejecutaremos desde un directorio donde estén presentes los ficheros de la práctica que se esté realizando. Por ejemplo, si hemos descargado los materiales de la práctica 1 en el directorio /home/nombre_usuario/datos/aoc/practicas/AOC-prac1-materiales, y tenemos el script en /home/nombre_usuario/datos/aoc/practicas/aoc-docker-image, los siguientes comandos nos permitirían compilar y ejecutar varios programas dentro del contenedor:

  $ cd /home/nombre_usuario/datos/aoc/practicas/AOC-prac1-materiales
  $ sudo /home/nombre_usuario/datos/aoc/practicas/aoc-docker-image/aoc-docker-image bash
  # cd peak-performance
  # make all
  # ./peak-freq-gcc
  # ./peak-flops-icc
  
Donde las líneas que comienzan con $ indican que el comando se ejecuta como usuario normal, y las que comienzan con # indican que el comando se ejecuta como root dentro del contenedor. No se muestra la salida de los comandos.

Se aconseja definir un alias (o hacer un pequeño script o función de bash) para ejecutar el script aoc-docker-image desde cualquier directorio. Por ejemplo con un comando similar a «alias a="sudo /home/nombre_usuario/datos/aoc/practicas/aoc-docker-image"». De esta forma, podremos ejecutar cómodamente la mayoría de los comandos casi como si estuviéramos trabajando sin Docker. Por ejemplo, escribiendo «a make all» y «a ./peak-freq-gcc» en el directorio de la práctica 1 compilaríamos todos los programas incluidos en los materiales y ejecutaríamos uno de ellos.

Obsérvese que los programas que se ejecutan en el contenedor lo hacen como root, por lo que los ficheros creados tendrán a root como propietario. Esto se puede cambiar posteriormente si se desea con el comando «chown».

** Uso de aoc-docker-image en Windows

Ver https://docs.microsoft.com/es-es/windows/wsl/tutorials/wsl-containers

Requisitos previos

1. Asegúrese de que la máquina Windows 10, actualizada a la versión 2004, compilación 18362 o posterior.
2. Habilite WSL, instale una distribución de Linux y actualice a WSL 2.
  - Paso 1: Habilitación del Subsistema de Windows para Linux
  	- Powershell: dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart
  - Paso 2: comprobación de los requisitos para ejecutar WSL 2
  	- Para sistemas x64: La versión 1903 o posterior, con la compilación 18362 o posterior.
	- Para sistemas ARM64: La versión 2004 o posterior, con la compilación 19041 o posterior.
	- Windows + R, escriba winver para ver la versión de windows. Se puede actualizar desde el menú de Configuración.
  - Paso 3: Habilitación de la característica Máquina virtual
  	- Requiere funciones de virtualización (https://docs.microsoft.com/es-es/windows/wsl/troubleshooting#error-0x80370102-the-virtual-machine-could-not-be-started-because-a-required-feature-is-not-installed)
  	- Powershell: dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart
  	- Reinicia la máquina para completar la instalación de WSL y la actualización a WSL 2.
  - Paso 4: Descarga del paquete de actualización del kernel de Linux
  	- Descargue la versión más reciente:
  		https://wslstorestorage.blob.core.windows.net/wslblob/wsl_update_x64.msi
  		https://wslstorestorage.blob.core.windows.net/wslblob/wsl_update_arm64.msi (versión ARM)
  	- Ejecuta el paquete de actualización que descargaste en el paso anterior. (Haga doble clic para ejecutarlo. Se le pedirán permisos elevados. Seleccione "Sí" para aprobar esta instalación).
  - Paso 5: Definición de WSL 2 como versión predeterminada
  	powershell: wsl --set-default-version 2
  - Paso 6: Instalación de la distribución de Linux que quiera
  	- Abre Microsoft Store (https://aka.ms/wslstore) y selecciona tu distribución de Linux favorita.
  		- Recomendado Ubuntu (https://www.microsoft.com/store/apps/9n6svws3rx71). Puede ser cualquiera.
  	- En la página de la distribución, selecciona "Obtener".
  	- La primera vez que inicies una distribución de Linux recién instalada, se abrirá una ventana de la consola y se te pedirá que esperes un minuto o dos para que los archivos se descompriman y se almacenen en tu equipo. Todos los inicios posteriores deberían tardar menos de un segundo en completarse.
  		- Tendrás que crear una cuenta de usuario y una contraseña para la nueva distribución de Linux.

3. Instalación de Terminal Windows (opcional)
 	- Terminal Windows permite habilitar varias pestañas (cambiar rápidamente entre varias líneas de comandos de Linux, el símbolo del sistema de Windows, PowerShell, la CLI de Azure, etc.), crear enlaces de teclado personalizados (teclas de método abreviado para abrir o cerrar pestañas, copiar y pegar, etc.), usar la característica de búsqueda y configurar temas personalizados (esquemas de colores, estilos y tamaños de fuente, imagen de fondo/desenfoque/transparencia)
 	- Seguir los pasos de https://docs.microsoft.com/es-es/windows/terminal/get-started.

4. Instale Visual Studio Code (opcional). Esto proporcionará la mejor experiencia, incluida la capacidad de codificar y depurar dentro de un contenedor de Docker remoto y conectado a la distribución de Linux. (https://code.visualstudio.com/download)

5. Regístrese para obtener un identificador de Docker en Docker Hub (opcional).

6. Instalación de Docker Desktop
  6.1 Descargue Docker Desktop (https://docs.docker.com/docker-for-windows/wsl/#download) y siga las instrucciones de instalación.
  	6.1.1 Descargue Docker Desktop Stable 2.3.0.2 (https://hub.docker.com/editions/community/docker-ce-desktop-windows/) o una versión posterior.
  	 	- Sigue las instrucciones de instalación habituales. Si estás ejecutando uno de los sistemas soportados, Docker Desktop te guiará para habilitar WSL 2 durante la instalación.
	6.1.2 Inicia Docker Desktop Desde el menú de inicio de windows.
	6.1.3 Desde el menú de Docker, seleccionar Settings > General.
    	6.1.4 Selecciona el check box de Use WSL 2 based engine.
		- Si el sistema ya soportaba WSL 2, la opción estará habilitada por defecto (y así debería ser de haber seguido todos los pasos).
	6.1.5 Click Apply & Restart.
	6.1.6 Asegurarse que la distribución se ejecuta en modo WSL 2. WSL puede ejecutar las distribuciones de linux en modo V1 o V2.
	    	- Para comprobar el modo de WSL, ejecutar en powershell: 
			- wsl.exe -l -v
		- Para actualizar tus distribuciones actuales a v2, ejecutar: 
			- wsl.exe --set-version (nombre de la distribución) 2
		- Para establecer V2 como la versión por defecto para futuras instalación:
			- wsl.exe --set-default-version 2
	6.1.7 Cuando Docker Desktop reinicie, ir a Settings > Resources > WSL Integration.
		- La integración de Docker-WSL se habilitará en tu distribución WSL por defecto. En nuestro caso Ubuntu. Para cambiar la WSL distro por defecto, ejecutar:
			- wsl --set-default <distro name>. Ejemplo: wsl --set-default ubuntu.
		- Opcionalmente, selecciona cualquier distribución adicional en la que te gustaría habilitar la integración Docker-WSL.
			- Nota: La integración Docker-WSL depende de glibc ejecutándose en tu distribución. Puede haber problemas en distros basadas a musl como Alpine.
	6.1.8 Click Apply & Restart.
  6.2 Una vez instalado, inicie Docker Desktop desde windows menú Inicio y, a continuación, seleccione el icono de Docker en el menú de iconos ocultos de la barra de tareas. Haga clic con el botón derecho en el icono para mostrar el menú comandos de Docker y seleccione "Configuración".
  6.3 Asegúrese de que la opción "Use the WSL 2 based engine" (Usar el motor basado en WSL 2) esté activada en Configuración > general.
  6.4 Seleccione entre las distribuciones de WSL 2 instaladas en las que desea habilitar la integración de Docker; para lo que debe ir a: Configuración > Recursos > Integración de WSL.
  6.5 Para confirmar que Docker se ha instalado, abra una distribución de WSL (por ejemplo, Ubuntu) (desde windows terminal por ejemplo) y muestre la versión y el número de compilación especificando: docker --version.
  6.6 Compruebe que la instalación funciona correctamente mediante la ejecución de una imagen de Docker integrada simple mediante: docker run hello-world
  6.7 a partir de ahora todos los contenedores de las distribuciones integradas con Docker Desktop aparecerán en Docker Desktop/Containers&Apps y sus imágenes en images.
  
7. Desde la terminal de Ubuntu (Windows Terminal) podemos seguir los pasos del apartado anterior (** Uso de aoc-docker-image en Linux) para compilar la imagen y el contenedor que se va a usar en estas prácticas. Esta imagen y contenedor aparecerá en Docker Desktop también. Se podrá ejecutar desde docker Desktop o desde la terminal de Ubuntu. La recomendación es hacerlo desde la propia terminal de ubuntu siguiendo los pasos del apartado anterior.

8. NOTA: En caso de usar virtualbox en Windows, pueden surgir problemas con las máquinas virtuales tras la activación de WSL y quedar inutilizadas. El problema reside en la baja compatibilidad entre los hypervisors utilizados por ambos sistemas de virtualización. Oracle se encuentra afinando este problema en cada versión de VirtualBox. Actualmente se ha documentado problemas de inicio de sesión en la interfaz gráfica de Ubuntu en una MV de virtualBox en la versión 6.1.26. Con las terminales de comandos no se ha percibido problema alguno. Por otra parte, VMWare Player no presenta incompatibilidad con Hyper-V usado en en WSL. Las máquinas virtuales de VMWare funcionan correctamente con WSL.


** Uso de aoc-docker-image en Mac OS X

Ver https://docs.docker.com/docker-for-mac/install/

El script debería funcionar si está instalado Docker.

