#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x092a35a2, "_copy_to_user" },
	{ 0x9f222e1e, "alloc_chrdev_region" },
	{ 0xd2554727, "cdev_init" },
	{ 0xdb375fb3, "cdev_add" },
	{ 0x326b4c7f, "class_create" },
	{ 0x160b81b4, "device_create" },
	{ 0xbd03ed67, "random_kmalloc_seed" },
	{ 0x9f568b3d, "kmalloc_caches" },
	{ 0xea8ca849, "__kmalloc_cache_noprof" },
	{ 0x9aa6980d, "mutex_init_generic" },
	{ 0xe8213e80, "_printk" },
	{ 0x0bc5fb0d, "unregister_chrdev_region" },
	{ 0x2e921116, "cdev_del" },
	{ 0x07a5cde6, "class_destroy" },
	{ 0xd17123e4, "device_destroy" },
	{ 0xcb8b6ec6, "kfree" },
	{ 0x092a35a2, "_copy_from_user" },
	{ 0xd272d446, "__fentry__" },
	{ 0x9aa6980d, "mutex_unlock" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0x7057d579, "mutex_trylock" },
	{ 0x5cb46e6d, "validate_usercopy_range" },
	{ 0xa61fd7aa, "__check_object_size" },
	{ 0xd954c786, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0x092a35a2,
	0x9f222e1e,
	0xd2554727,
	0xdb375fb3,
	0x326b4c7f,
	0x160b81b4,
	0xbd03ed67,
	0x9f568b3d,
	0xea8ca849,
	0x9aa6980d,
	0xe8213e80,
	0x0bc5fb0d,
	0x2e921116,
	0x07a5cde6,
	0xd17123e4,
	0xcb8b6ec6,
	0x092a35a2,
	0xd272d446,
	0x9aa6980d,
	0xd272d446,
	0x7057d579,
	0x5cb46e6d,
	0xa61fd7aa,
	0xd954c786,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"_copy_to_user\0"
	"alloc_chrdev_region\0"
	"cdev_init\0"
	"cdev_add\0"
	"class_create\0"
	"device_create\0"
	"random_kmalloc_seed\0"
	"kmalloc_caches\0"
	"__kmalloc_cache_noprof\0"
	"mutex_init_generic\0"
	"_printk\0"
	"unregister_chrdev_region\0"
	"cdev_del\0"
	"class_destroy\0"
	"device_destroy\0"
	"kfree\0"
	"_copy_from_user\0"
	"__fentry__\0"
	"mutex_unlock\0"
	"__x86_return_thunk\0"
	"mutex_trylock\0"
	"validate_usercopy_range\0"
	"__check_object_size\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "A76F3F77DDE24BE6F86EE18");
