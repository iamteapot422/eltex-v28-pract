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
	{ 0xd272d446, "__fentry__" },
	{ 0x5cb46e6d, "validate_usercopy_range" },
	{ 0xa61fd7aa, "__check_object_size" },
	{ 0x092a35a2, "_copy_to_user" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0x092a35a2, "_copy_from_user" },
	{ 0x26772f20, "remove_proc_entry" },
	{ 0xcb8b6ec6, "kfree" },
	{ 0xdef8136e, "proc_create" },
	{ 0xbd03ed67, "random_kmalloc_seed" },
	{ 0xc4fee520, "kmalloc_caches" },
	{ 0x4574d0c7, "__kmalloc_cache_noprof" },
	{ 0xe9196a28, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xd272d446,
	0x5cb46e6d,
	0xa61fd7aa,
	0x092a35a2,
	0xd272d446,
	0x092a35a2,
	0x26772f20,
	0xcb8b6ec6,
	0xdef8136e,
	0xbd03ed67,
	0xc4fee520,
	0x4574d0c7,
	0xe9196a28,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"__fentry__\0"
	"validate_usercopy_range\0"
	"__check_object_size\0"
	"_copy_to_user\0"
	"__x86_return_thunk\0"
	"_copy_from_user\0"
	"remove_proc_entry\0"
	"kfree\0"
	"proc_create\0"
	"random_kmalloc_seed\0"
	"kmalloc_caches\0"
	"__kmalloc_cache_noprof\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "DBDBA23B3717A86C71F8784");
