
extern int main(void);

__attribute__((section(".startup"), used))
void init(void)
{
    main();
}

