DEF_CMD(push, 1, 1, 
{
    detectPush (commandsArray, &i, registers, stack, ramElements);
})

DEF_CMD(add, 2, 0,
{
    StackPush (stack, StackPop (stack) + StackPop (stack));
})

DEF_CMD(sub, 3, 0,
{
    StackPush (stack, StackPop (stack) - StackPop (stack));
})

DEF_CMD(mul, 4, 0,
{
    StackPush (stack, StackPop (stack) * StackPop (stack));
})

DEF_CMD(div, 5, 0,
{
    StackPush (stack, StackPop (stack) / StackPop (stack));
})

DEF_CMD(in, 6, 1,
{
    detectPush (commandsArray, &i, registers, stack, ramElements);
})

DEF_CMD(out, 7, 0,
{
    printf ("%d", StackPop (stack));
})

DEF_CMD(hlt, 8, 0,
{
    exit (1);
})

DEF_CMD(pop, 9, 1,
{
    detectPush (commandsArray, &i, registers, stack, ramElements);
})

DEF_CMD(jb, 10, <)

DEF_JMP(jbe, 11, <=)

DEF_JMP(ja, 12, >)

DEF_JMP(jae, 13, >=)

DEF_JMP(je, 14, ==)

DEF_JMP(jne, 15, 1, !=)

DEF_CMD(endl, 16, 0,
{
    printf ("\n");
})

DEF_CMD(space, 17, 0,
{
    printf (" ");
})

