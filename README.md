一、背景

你正在为一台嵌入式设备设计简化的任务调度系统。



设备可能同时接收温度传感器报警、电机控制、通信请求等任务。不同任务具有不同的优先级，调度器必须优先执行最重要的任务。



系统不仅要执行任务，还必须支持任务查询、取消、恢复、历史记录、文件持久化和状态备份。



你需要设计一个名为 EdgeOS 的程序，实现上述功能。

二、任务类型与优先级

系统支持两种任务，所有任务具有三个公共属性：

属性	类型	说明

id	std::string	唯一任务编号

basePriority	int	基础优先级

sequence	size\_t	创建顺序，从 0 开始递增



两种任务的具体规则如下。

类型 A：SensorTask（传感器任务）



包含 value（测量值）和 threshold（报警阈值）。



优先级计算公式：

\\\[

P=basePriority+\\max(0,value-threshold)

\\]



例如基础优先级为 4、测量值 13、阈值 10，则最终优先级为 7。



类型 B：ControlTask（控制任务）



包含 urgency（紧急程度）。



优先级计算公式：

\\\[

P=basePriority+2\\times urgency

\\]



例如基础优先级为 2，紧急程度为 3，则最终优先级为 8。





任务执行顺序遵循：

1\. 最终优先级较大的任务先执行。

2\. 如果优先级相同，则创建顺序较早的任务先执行。

3\. 已经取消或完成的任务不能再次直接执行。

basePriority、value、threshold 的范围均为 0～100，urgency 的范围为 0～10。

三、必须实现的功能

第一部分：基础任务管理（25 分）

程序从标准输入读取命令。

命令	功能

ADD\_S id base value threshold	添加传感器任务

ADD\_C id base urgency	添加控制任务

FIND id	查询任务

CANCEL id	取消尚未执行的任务

STATS	统计任务数量



任务只有三种状态：PENDING、DONE 和 CANCELED。



所有 id 在同一次程序运行期间必须全局唯一，即使任务已经完成或被取消，也不能再次使用其编号。



非法操作统一输出 ERROR，例如重复添加 ID、取消已执行任务、取消不存在的任务。非法操作不得改变系统状态。



FIND 查询成功时，输出：

FIND <id> <status> <priority>



找不到任务时输出 NOT\_FOUND。



STATS 输出等待、已完成和已取消的任务数量：

STATS <pending> <done> <canceled>



第二部分：优先队列调度（20 分）

实现以下命令：

命令	功能

PEEK	查看当前最高优先级任务

RUN	执行当前最高优先级任务

LIST k	按执行顺序查看前 k 个待执行任务

其中 RUN 必须真正移除一个待执行任务，并将其状态更新为 DONE。

这里有一个核心限制：

不允许使用 std::priority\_queue。必须自行实现二叉堆。

可以使用 std::vector 作为底层连续存储空间，但插入、删除堆顶和堆调整必须自行实现。

要求：

\- 插入任务：\\(O(\\log n)\\)

\- 删除有效堆顶：均摊 \\(O(\\log n)\\)

\- PEEK 不得执行任务。

\- LIST 不得改变原调度器。

你还需要考虑一个问题：

如果任务已经进入二叉堆，之后被 CANCEL，应该怎样处理堆中已经存在的任务？

允许采用惰性删除，但必须保证不会执行已取消任务，也不能因为无效任务长期留在堆顶而返回错误结果。

第三部分：任务索引与数据结构（20 分）

这一部分专门结合 Weiss 第 3～6 章。

A. 散列表

自行实现一个以任务 ID 为键的散列表，使用链地址法解决冲突。

必须支持插入、查找和读取任务状态。允许使用 std::vector 和 std::list，但本部分不得直接用 std::unordered\_map 替代散列表实现。

目标是平均 \\(O(1)\\) 的任务查询。

B. AVL 树

增加一个命令：

IDS left right

输出 ID 位于闭区间 \[left, right] 内的所有 PENDING 任务，按照 ID 字典序升序输出。

例如：

IDS s1 s9

要求利用自行实现的 AVL 树完成有序查询。AVL 树中仅索引 PENDING 任务，执行、取消和恢复任务时要同步更新索引。

目标复杂度为 \\(O(\\log n+k)\\)，其中 \\(k\\) 是输出任务数量。

这意味着你需要让二叉堆、散列表和 AVL 树共同管理同一批任务，但避免制造三份互不关联的任务实体。



第四部分：撤销与历史记录（10 分）

实现：

UNDO

HISTORY k



UNDO 撤销最近一次尚未撤销的成功 RUN，让该任务重新进入 PENDING 状态，并重新参加优先级调度。

HISTORY k 按从近到远的顺序，输出至多 k 条尚未撤销的执行记录。

本部分应使用栈结构实现。

需要注意，CANCEL 不是 RUN，不进入撤销栈；执行失败也不能产生历史记录。

UNDO 在无可撤销记录时输出 EMPTY。

第五部分：面向对象与资源管理（15 分）

这一部分重点考查《C++ Primer》第 12～15 章。

必须满足以下设计要求：

\- 设计一个抽象基类 Task，由 SensorTask 和 ControlTask 公有继承。

\- 通过虚函数实现不同任务的优先级计算。

\- 基类使用虚析构函数。

\- 使用 override，并正确处理 public、protected 和 private。

\- 任务对象的所有权由智能指针管理，不得依赖手工遍历 delete。

\- 为调度器设计正确的拷贝构造函数和拷贝赋值运算符，要求复制后的任务实体互相独立。

\- 设计适当的比较运算符，使任务优先级与创建顺序的比较逻辑能够复用。

\- 设计输出运算符 operator<<，用于输出任务的详细信息。

不能仅仅让程序运行成功，还要保证复制调度器之后，对其中一份进行修改不会影响另一份。

第六部分：文件 I/O 与系统状态（10 分）

实现：

SAVE filename

LOAD filename

SNAPSHOT

RESTORE

要求：

SAVE 将所有任务属性、类型、状态和创建序号保存到文本文件。

LOAD 从文件重建任务数据、优先队列、散列表和 AVL 树。加载成功后清空执行历史，并将下一创建序号设为所有已有序号最大值加一。如果文件打开或数据校验失败，原状态必须保持不变。

SNAPSHOT 保存当前系统的一份独立深拷贝。

RESTORE 恢复最近保存的快照，包含任务状态、堆、索引和执行历史。快照本身不被消耗，可以重复恢复。再次 SNAPSHOT 会覆盖旧快照。

这些操作成功时分别输出 SAVED、LOADED、SNAPSHOT\_OK、RESTORED；失败时输出 ERROR。

四、输入输出示例

第一行为命令数量 Q，之后每行一条命令。

任务 ID 只包含英文字母、数字和下划线，长度不超过 32。Q 最大为 100000。

样例输入

&#x20;复制

14

ADD\_S s1 4 13 10

ADD\_C c1 2 3

ADD\_S s2 5 17 15

PEEK

LIST 3

RUN

CANCEL s1

FIND s1

UNDO

PEEK

RUN

RUN

RUN

STATS

预期输出

PEEK c1 8

LIST c1:8 s1:7 s2:7

RUN c1 8

CANCEL s1

FIND s1 CANCELED 7

UNDO c1

PEEK c1 8

RUN c1 8

RUN s2 7

EMPTY

STATS 0 2 1

输出约定：成功添加任务不输出信息；PEEK、RUN 在没有待执行任务时输出 EMPTY；LIST、HISTORY、IDS 以命令名开头，后接空格分隔的 id:priority 项，若结果为空仅输出 EMPTY。

