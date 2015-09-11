# plant77
La idea principal del sistema Plant77 es el automantenimiento de un indoor u outdoor para plantas.
El sistema consta de una serie de conexiones 220v para poder iniciar o cortar:
- Luz
- Aire
- Extractor

Tambien posee entradas para conexiones de sistemas de control de temperatura, humedad de la planta y control del riego (teoricamente por 12v por esto lo apartamos del modulo 220v).

Este sistema esta planteado en una plataforma Arduino y una shield ethernet con SD. El modulo genera una conexión TCP por un puerto (default 192.168.X.77), por donde se podrá acceder a un sitio web el cual nos dara el entorno de configuración y reporte del sistema.

Las configuraciones se almacenana en un archivo, el cual podra ser consultado por Arduino para configurarse en su inicio o para editar su propia configuración, dependiendo de los datos ingresados por el usuario.