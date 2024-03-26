#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL v2");

#define MINOR_BASE	0
#define MINOR_NUMBER 1

static struct class * tsukutsuku_mod_class;
static struct cdev tsukutsuku_mod_cdev;
static dev_t dev;

typedef struct _automaton_state {
	char *msg;
	int msg_len;
	int n;
	int *transition;
	int *transition_prob;
} automaton_state;

static char m0[] = "ジー...";
static int t0[] = { 1 };
static int tp0[] = { 100 };

static char m1[] = "ツクツク";
static int t1[] = { 1, 2 };
static int tp1[] = { 35, 65 };

static char m2[] = "ボーシ！";
static int t2[] = { 3 };
static int tp2[] = { 100 };

static char m3[] = "ツクツクボーシ！";
static int t3[] = { 3, 4 };
static int tp3[] = { 90, 10 };

static char m4[] = "ウイヨース！";
static int t4[] = { 4, 0 };
static int tp4[] = { 80, 20 };

static automaton_state tsukutsuku_automaton[] = {
	{
		.msg = m0,
		.msg_len = sizeof(m0),
		.n = sizeof(t0),
		.transition = t0,
		.transition_prob = tp0,
	},
	{
		.msg = m1,
		.msg_len = sizeof(m1),
		.n = sizeof(t1),
		.transition = t1,
		.transition_prob = tp1,
	},
	{
		.msg = m2,
		.msg_len = sizeof(m2),
		.n = sizeof(t2),
		.transition = t2,
		.transition_prob = tp2,
	},
	{
		.msg = m3,
		.msg_len = sizeof(m3),
		.n = sizeof(t3),
		.transition = t3,
		.transition_prob = tp3,
	},
	{
		.msg = m4,
		.msg_len = sizeof(m4),
		.n = sizeof(t4),
		.transition = t4,
		.transition_prob = tp4,
	},
};

static int current_state_num = 0;

static ssize_t read_tsukutsuku(struct file *file, char __user *buf, size_t size, loff_t *offset) {
	automaton_state current_state = tsukutsuku_automaton[current_state_num];
	char *buffer = current_state.msg;
	size_t chunk = min(current_state.msg_len, size);
	unsigned int random, prob;
	int i, prob_sum;

	if (chunk <= 0 || size <= 0) {
		return 0;
	}

	if (copy_to_user(buf, buffer, chunk)) {
		return -EFAULT;
	}

	get_random_bytes(&random, sizeof(random));
	prob = random % 100;

	prob_sum = 0;
	for (i = 0; i < current_state.n; i ++) {
		prob_sum += current_state.transition_prob[i];
		if (prob < prob_sum) {
			break;
		}
	}
	current_state_num = current_state.transition[i]; // update state

	return chunk;
}

static struct file_operations tsukutsuku_mod_drv_fops = {
	.owner		= THIS_MODULE,
	.open		= NULL,
	.release	= NULL,
	.read		= read_tsukutsuku,
	.write		= NULL,
};

static int __init tsukutsuku_mod_init(void)
{
	int result;
	struct device *tsukutsuku_mod_dev = NULL;

	pr_info("tsukutsuku_mod: init function start\n");

	/* Major/Minor番号を動的に確保し/proc/devicesへ登録 */
	result = alloc_chrdev_region(&dev, MINOR_BASE, MINOR_NUMBER, "tsukutsuku_mod");
	
	if(result){
		pr_err("tsukutsuku_mod: error, alloc_chrdev_region = %d\n", result);
		goto MMNUMBER_ERR;
	}

	/* モジュールのクラスを/sys/class/へ登録 */
	tsukutsuku_mod_class = class_create(THIS_MODULE, "tsukutsuku_mod");
	if(IS_ERR(tsukutsuku_mod_class)){
		result = PTR_ERR(tsukutsuku_mod_class);
		pr_err("tsukutsuku_mod: error, class_create = %d\n", result);
		goto CLASS_ERR;
	}
	
	/* キャラクタデバイス構造体（cdev構造体）の初期化およびfopsの設定 */
	cdev_init(&tsukutsuku_mod_cdev, &tsukutsuku_mod_drv_fops);

	/* キャラクタデバイス構造体の登録 */
	result = cdev_add(&tsukutsuku_mod_cdev, dev, MINOR_NUMBER);
	if(result){
		pr_err("tsukutsuku_mod: error, cdev_add = %d\n", result);
		goto CDEV_ERR;
	}

	/* デバイスノードの作成．作成したノードは/dev以下からアクセス可能 */
	tsukutsuku_mod_dev = device_create(tsukutsuku_mod_class, NULL, dev, NULL, "tsukutsuku");
	if(IS_ERR(tsukutsuku_mod_dev)){
		result = PTR_ERR(tsukutsuku_mod_dev);
		pr_err("tsukutsuku_mod: error, device_create = %d\n", result);
		goto DEV_ERR;
	}
	pr_info("tsukutsuku_mod: init function end\n");
	return result;

DEV_ERR:
	cdev_del(&tsukutsuku_mod_cdev);
CDEV_ERR:
	class_destroy(tsukutsuku_mod_class);
CLASS_ERR:
	unregister_chrdev_region(dev, MINOR_NUMBER);
MMNUMBER_ERR:
	return result;
}

static void __exit tsukutsuku_mod_exit(void)
{
	pr_info("tsukutsuku_mod: exit function start\n");

	/* デバイスノードの削除 */
	device_destroy(tsukutsuku_mod_class, dev);

	/* キャラクタデバイスをKernelから削除 */
	cdev_del(&tsukutsuku_mod_cdev);

	/* デバイスのクラス登録を解除 */
	class_destroy(tsukutsuku_mod_class);

	/* デバイスが使用していたメジャー番号の登録を解除 */
	unregister_chrdev_region(dev, MINOR_NUMBER);

	pr_info("tsukutsuku_mod: exit function end\n");
}

module_init(tsukutsuku_mod_init);
module_exit(tsukutsuku_mod_exit);
