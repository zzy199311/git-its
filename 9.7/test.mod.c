#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x731859dc, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xfcf95068, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x26b8c406, __VMLINUX_SYMBOL_STR(skb_clone) },
	{ 0x493e14af, __VMLINUX_SYMBOL_STR(__pskb_pull_tail) },
	{ 0x50eedeb8, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xb4390f9a, __VMLINUX_SYMBOL_STR(mcount) },
	{ 0xfdf9e033, __VMLINUX_SYMBOL_STR(skb_push) },
	{ 0x59ef7e82, __VMLINUX_SYMBOL_STR(__alloc_skb) },
	{ 0xf0fdf6cb, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0x9e6ce1f9, __VMLINUX_SYMBOL_STR(kfree_skb) },
	{ 0xebacdff8, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0xc0dad23a, __VMLINUX_SYMBOL_STR(consume_skb) },
	{ 0xe914e41e, __VMLINUX_SYMBOL_STR(strcpy) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "6DC63971FB99597698D6EA4");
