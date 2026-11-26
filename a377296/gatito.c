#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jacqueline Helo Flores");
MODULE_DESCRIPTION("Modulo de kernel, tema -> gatos");

#define DEVICE_NAME "gatito"
#define BUFFER_SIZE 1024

static int ya_maullo = 0;
static int ronroneo = 0;
static int major_number;

static char maullido[BUFFER_SIZE] = "Miau miau! Soy un gatito que vive dentro del kernel\n";
static char caricia[BUFFER_SIZE];

static int gatito_open(struct inode *, struct file *);
static int gatito_close(struct inode *, struct file *);
static ssize_t gatito_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t gatito_write(struct file *, const char __user *, size_t, loff_t *);

static struct file_operations fops = {
    .open = gatito_open,
    .release = gatito_close,
    .read = gatito_read,
    .write = gatito_write
};

static ssize_t gatito_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    int bytes_to_read = strlen(maullido);

    if (ya_maullo == 1) {
        ya_maullo = 0;
        return 0;
    }

    if (ronroneo == 0) {
        ya_maullo = 1;
    }

    if (copy_to_user(buf, maullido, bytes_to_read) != 0) {
        ya_maullo = 0;
        printk(KERN_ERR "No se pudo copiar el maullido al usuario\n");
        return -EFAULT;
    }

    return bytes_to_read;
}

static ssize_t gatito_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset)
{
    if (len >= BUFFER_SIZE) {
        len = BUFFER_SIZE - 1;
    }

    if (copy_from_user(caricia, buffer, len) != 0) {
        return -EFAULT;
    }

    caricia[len] = '\0';

    printk(KERN_INFO "Recibi %d bytes del usuario\n", (int)len);
    printk(KERN_INFO "La cadena recibida es: %s\n", caricia);

    if (caricia[0] == '1') {
        ronroneo = 1;
        printk(KERN_INFO "Modo flujo ACTIVADO, el gatito ronronea\n");
    } else if (caricia[0] == '0') {
        ronroneo = 0;
        printk(KERN_INFO "Modo flujo DESACTIVADO, el gatito se calla\n");
    }

    return len;
}

static int gatito_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Se abrio el dispositivo %s\n", DEVICE_NAME);
    return 0;
}

static int gatito_close(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "El  gatito se fue a dormir\n");
    return 0;
}

static int __init gatito_load(void)
{
    printk(KERN_INFO "Ya llego el gatito a ronronear\n");

    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "No se pudo registrar el major number\n");
        return major_number;
    }

    printk(KERN_INFO "Me asignaron el major number %d\n", major_number);
    printk(KERN_INFO "Crea el archivo de dispositivo con:\n");
    printk(KERN_INFO "Mknod /dev/%s c %d 0\n", DEVICE_NAME, major_number);

    return 0;
}

static void __exit gatito_unload(void)
{
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "Adios, el gatito se escapo por la ventana\n");
}

module_init(gatito_load);
module_exit(gatito_unload);
