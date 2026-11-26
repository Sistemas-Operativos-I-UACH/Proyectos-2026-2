#include <linux/module.h>   // Requerido por todos los módulos
#include <linux/kernel.h>   // printk y KERN_INFO
#include <linux/fs.h>       // struct file_operations, inode
#include <linux/cdev.h>     // cdev
#include <linux/device.h>   // class_create / device_create
#include <linux/uaccess.h>  // copy_to_user / copy_from_user
#include <linux/delay.h>    // msleep_interruptible
#include <linux/string.h>
#include <linux/err.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Erick Efraín Orozco Ríos");
MODULE_DESCRIPTION("Modulo chopper: muestra mensajes, recibe cadenas y crea /dev/chopper");

#define DEVICE_NAME     "chopper"
#define CLASS_NAME      "chopperclass"
#define BUFFER_SIZE     1024
#define STREAM_DELAY_MS 500

static int stream = 0;                 /* 0 = flujo desactivado (default), 1 = activado */
static dev_t device_numbers;
static struct class *my_class;
static struct cdev my_cdev;
static int device_open_count = 0;

/* Mensaje que se muestra en espacio de usuario (distinto al del ejemplo) */
static char kernel_buffer[BUFFER_SIZE] = "Soy Chopper, el doctor de los Sombreros de Paja!\n";
static char write_buffer[BUFFER_SIZE];

static int     chopper_open(struct inode *, struct file *);
static int     chopper_close(struct inode *, struct file *);
static ssize_t chopper_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t chopper_write(struct file *, const char __user *, size_t, loff_t *);

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = chopper_open,
    .release = chopper_close,
    .read    = chopper_read,
    .write   = chopper_write
};

/*
 * Lectura del dispositivo (ej. cat /dev/chopper).
 *  - Flujo desactivado: devuelve el mensaje una vez y luego EOF.
 *  - Flujo activado: devuelve el mensaje una y otra vez (cada 500 ms)
 *    hasta que el usuario presione CTRL-C.
 */
static ssize_t chopper_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    size_t msg_len = strlen(kernel_buffer);

    if (len < msg_len)
        msg_len = len;

    if (stream) {
        printk(KERN_INFO "chopper: Stream Activado\n");
        /* Devuelve distinto de 0 si llega una señal (CTRL-C) */
        if (msleep_interruptible(STREAM_DELAY_MS))
            return -ERESTARTSYS;
    } else {
        printk(KERN_INFO "chopper: Stream Desactivado\n");
        if (*off > 0)       /* ya se entregó el mensaje: fin de archivo */
            return 0;
    }

    if (copy_to_user(buf, kernel_buffer, msg_len) != 0) {
        printk(KERN_ERR "chopper: Failed to copy data to user\n");
        return -EFAULT;
    }

    if (!stream)
        *off += msg_len;

    return msg_len;
}

/*
 * Escritura al dispositivo (ej. echo "hola" > /dev/chopper).
 *  - Muestra la cadena recibida en la bitácora (dmesg).
 *  - "1" activa el modo flujo, "0" lo desactiva.
 */
static ssize_t chopper_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset)
{
    size_t n = len;

    printk(KERN_INFO "chopper: Entrando a write\n");

    if (n > BUFFER_SIZE - 1)
        n = BUFFER_SIZE - 1;

    if (copy_from_user(write_buffer, buffer, n) != 0)
        return -EFAULT;

    write_buffer[n] = '\0';

    /* Quitar el salto de línea que agrega echo */
    if (n > 0 && write_buffer[n - 1] == '\n')
        write_buffer[n - 1] = '\0';

    printk(KERN_INFO "chopper: Received %zu bytes from the user\n", len);
    printk(KERN_INFO "chopper: Received \"%s\" from the user\n", write_buffer);

    if (strcmp(write_buffer, "1") == 0) {
        stream = 1;
        printk(KERN_INFO "chopper: Modo flujo ACTIVADO\n");
    } else if (strcmp(write_buffer, "0") == 0) {
        stream = 0;
        printk(KERN_INFO "chopper: Modo flujo DESACTIVADO\n");
    }

    printk(KERN_INFO "chopper: Saliendo de write\n");
    return len;
}

static int chopper_open(struct inode *inode, struct file *file)
{
    device_open_count++;
    printk(KERN_INFO "chopper: Se abrió el dispositivo %s (%d veces)\n",
           DEVICE_NAME, device_open_count);
    return 0;
}

static int chopper_close(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "chopper: Cerrando el dispositivo %s\n", DEVICE_NAME);
    return 0;
}

static int __init chopper_load(void)
{
    int ret;

    printk(KERN_INFO "chopper: Modulo cargado, el doctor ya esta en el barco\n");

    /* 1. Reservar numeros mayor y menor */
    ret = alloc_chrdev_region(&device_numbers, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        printk(KERN_ALERT "chopper: Device Number could not be allocated\n");
        return ret;
    }
    printk(KERN_INFO "chopper: Device Numbers Major: %d, Minor: %d registered\n",
           MAJOR(device_numbers), MINOR(device_numbers));

    /* 2. Crear la clase del dispositivo */
    my_class = class_create(CLASS_NAME);
    if (IS_ERR(my_class)) {
        printk(KERN_ALERT "chopper: Class could not be created\n");
        ret = PTR_ERR(my_class);
        goto ClassError;
    }

    /* 3. Inicializar y registrar el cdev en el kernel */
    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;
    ret = cdev_add(&my_cdev, device_numbers, 1);
    if (ret < 0) {
        printk(KERN_ALERT "chopper: Registering of device to kernel failed\n");
        goto AddError;
    }

    /* 4. Crear el archivo /dev/chopper (equivale a mknod) */
    if (IS_ERR(device_create(my_class, NULL, device_numbers, NULL, DEVICE_NAME))) {
        printk(KERN_ALERT "chopper: Device could not be created\n");
        ret = -ENOMEM;
        goto FileError;
    }

    return 0;

FileError:
    cdev_del(&my_cdev);
AddError:
    class_destroy(my_class);
ClassError:
    unregister_chrdev_region(device_numbers, 1);
    return ret;
}

static void __exit chopper_unload(void)
{
    device_destroy(my_class, device_numbers);
    cdev_del(&my_cdev);
    class_destroy(my_class);
    unregister_chrdev_region(device_numbers, 1);
    printk(KERN_INFO "chopper: Adios, nos vemos pronto!\n");
}

module_init(chopper_load);
module_exit(chopper_unload);
