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
	{ 0xd272d446, "__stack_chk_fail" },
	{ 0xdd6830c7, "sprintf" },
	{ 0x7ec472ba, "fg_console" },
	{ 0x1b0aeac3, "vc_cons" },
	{ 0x5a844b26, "__x86_indirect_thunk_r12" },
	{ 0x534ed5f3, "__msecs_to_jiffies" },
	{ 0x058c185a, "jiffies" },
	{ 0x32feeafc, "mod_timer" },
	{ 0x90a48d82, "__ubsan_handle_out_of_bounds" },
	{ 0xcc9483d5, "kernel_kobj" },
	{ 0x9fec3e3f, "kobject_create_and_add" },
	{ 0x3d3299c8, "sysfs_create_file_ns" },
	{ 0x02f9bbf0, "timer_init_key" },
	{ 0xa470b7e2, "add_timer" },
	{ 0x45f86ffc, "kobject_put" },
	{ 0x2352b148, "timer_delete_sync" },
	{ 0x0f2f972e, "param_ops_uint" },
	{ 0xd272d446, "__fentry__" },
	{ 0xbd03ed67, "__ref_stack_chk_guard" },
	{ 0x8e142c2e, "kstrtouint" },
	{ 0xe8213e80, "_printk" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xd954c786, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xd272d446,
	0xdd6830c7,
	0x7ec472ba,
	0x1b0aeac3,
	0x5a844b26,
	0x534ed5f3,
	0x058c185a,
	0x32feeafc,
	0x90a48d82,
	0xcc9483d5,
	0x9fec3e3f,
	0x3d3299c8,
	0x02f9bbf0,
	0xa470b7e2,
	0x45f86ffc,
	0x2352b148,
	0x0f2f972e,
	0xd272d446,
	0xbd03ed67,
	0x8e142c2e,
	0xe8213e80,
	0xd272d446,
	0xd954c786,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"__stack_chk_fail\0"
	"sprintf\0"
	"fg_console\0"
	"vc_cons\0"
	"__x86_indirect_thunk_r12\0"
	"__msecs_to_jiffies\0"
	"jiffies\0"
	"mod_timer\0"
	"__ubsan_handle_out_of_bounds\0"
	"kernel_kobj\0"
	"kobject_create_and_add\0"
	"sysfs_create_file_ns\0"
	"timer_init_key\0"
	"add_timer\0"
	"kobject_put\0"
	"timer_delete_sync\0"
	"param_ops_uint\0"
	"__fentry__\0"
	"__ref_stack_chk_guard\0"
	"kstrtouint\0"
	"_printk\0"
	"__x86_return_thunk\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "5B8C8503D8AABE009852AC6");
