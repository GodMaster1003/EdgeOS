# EdgeOS

**基于 C++11 的任务调度系统模拟器**

*A C++11 Task Scheduling Simulator*

**当前版本：** v1.0  
**项目性质：** C++ / 数据结构与算法 / 面向对象程序设计综合实践  
**开发环境：** Linux、C++11  
**项目状态：** 核心功能已实现，处于测试完善与工程化改进阶段

## 1. 项目简介（Overview）

EdgeOS 是一个使用 C++11 编写的简化任务调度系统模拟器。

本项目以嵌入式设备的任务管理场景为背景：设备可能需要处理传感器报警、控制指令等不同类型的任务，系统需要按照任务优先级决定处理顺序，同时支持任务查询、取消、撤销、历史记录与状态持久化。

项目的主要目的不是构建一个真正的操作系统，而是通过一个相对完整的应用场景，将数据结构、算法分析、面向对象设计与资源管理结合起来。

目前 EdgeOS 是一个单线程命令行模拟程序，不涉及真正的 CPU 上下文切换、抢占式调度或硬件控制。

## 2. 项目目标（Objectives）

本项目主要围绕以下目标展开：

1. **数据结构的自主实现：** 实现二叉堆、AVL 树、散列表和历史栈，并将其应用于任务管理。
2. **面向对象设计：** 通过抽象基类、继承、动态多态及虚函数实现不同任务类型。
3. **资源与生命周期管理：** 使用 RAII、智能指针管理任务对象，实现独立的调度器深拷贝。
4. **多索引一致性：** 使用不同数据结构索引同一批任务，并维护任务状态与索引之间的一致性。
5. **文件持久化：** 实现任务状态序列化、反序列化及系统状态快照。
6. **工程实践：** 逐步引入单元测试、异常安全检查、构建系统和代码审查机制。

## 3. 系统功能（Features）

### 3.1 任务类型

系统支持两种任务类型。

**SensorTask（传感器任务）**

属性包括任务 ID、基础优先级、测量值、报警阈值和创建序号。

最终优先级计算公式：

`priority = base_priority + max(0, value - threshold)`

**ControlTask（控制任务）**

属性包括任务 ID、基础优先级、紧急程度和创建序号。

最终优先级计算公式：

`priority = base_priority + 2 * urgency`

调度规则：

- 最终优先级越高，越先执行。
- 优先级相同时，优先执行创建时间更早的任务。
- 已完成或已取消的任务不参与正常调度。

任务具有三种状态：`PENDING`、`DONE`、`CANCELED`。

### 3.2 已实现的核心功能

| 功能 | 使用的数据结构或机制 |
|---|---|
| 添加与查询任务 | Hash_Table |
| 优先级调度 | BinaryHeap |
| 按任务 ID 范围查询 | AVL |
| 执行历史与撤销 | History_Stack |
| 任务所有权管理 | std::unique_ptr |
| 不同任务的优先级计算 | 虚函数与动态多态 |
| 调度器深拷贝 | clone() 与索引重建 |
| 状态保存与加载 | 文本文件序列化 |
| 系统快照及恢复 | 独立 Scheduler 副本 |

其中，散列表实现了动态扩容及桶树化，二叉堆采用惰性删除处理已取消任务。

## 4. 系统架构（Architecture）

### 4.1 目录结构

```text
EdgeOS/
├── include/
│   ├── Task.hpp
│   ├── Scheduler.hpp
│   ├── BinaryHeap.hpp
│   ├── HashTable.hpp
│   ├── AVL.hpp
│   ├── HistoryStack.hpp
│   ├── CommandProcessor.hpp
│   └── FileStorager.hpp
├── src/
│   ├── Task.cpp
│   ├── Scheduler.cpp
│   ├── BinaryHeap.cpp
│   ├── HashTable.cpp
│   ├── AVL.cpp
│   ├── HistoryStack.cpp
│   ├── CommandProcessor.cpp
│   ├── FileStorager.cpp
│   └── main.cpp
├── examples/
│   ├── basic.in
│   ├── basic.expected
│   ├── edge_cases.in
│   ├── edge_cases.expected
│   ├── snapshot.in
│   └── snapshot.expected
└── README.md
```

`examples/` 为准备添加的测试用例目录，不属于原始 v1.0 源码。

### 4.2 模块职责

**Task**

定义任务的公共接口，由 `SensorTask` 和 `ControlTask` 继承。

通过 `finalPriority()` 实现不同任务的优先级计算，并通过 `clone()` 支持多态对象复制。

**Scheduler**

系统的核心协调模块，负责任务创建、执行、取消、恢复，以及维护不同索引结构。

任务实体由 `std::vector<std::unique_ptr<Task>>` 管理，其他数据结构保存非拥有型任务指针。

**BinaryHeap**

自主实现最大堆，根据最终优先级及创建顺序决定任务执行次序。

取消任务时允许采用惰性删除，在访问堆顶时清理失效元素。

**Hash_Table**

以任务 ID 为键，负责对所有状态的任务进行查询。支持散列冲突处理、扩容及桶树化。

**AVL**

为所有待执行任务建立按 ID 排序的索引，支持闭区间范围查询。

**History_Stack**

使用栈保存成功执行的任务记录，支持历史查询和最近一次执行操作的撤销。

**Word_Processor**

解析输入命令，校验参数数量及格式，并分派给相应的系统模块。

**File_Storager**

负责文件保存、加载，以及具名系统快照的创建与恢复。

### 4.3 核心设计问题

系统中的一个任务同时可能受到二叉堆、散列表和 AVL 树的管理。

为了避免复制三份任务对象，EdgeOS 将任务所有权统一交给 Scheduler，其余数据结构仅维护任务引用。

当任务状态发生变化时，需要同步维护相关索引。

这是本项目需要重点研究和验证的设计问题之一。

## 5. 编译与运行（Build and Run）

建议使用支持 C++11 的 GCC 或 Clang。

在项目根目录执行：

```bash
g++ -std=c++11 -O2 -Wall -Wextra -Wpedantic src/*.cpp -o edgeos
./edgeos
```

程序逐行读取命令，输入 `QUIT` 或结束标准输入即可退出。

当前版本尚未实现原始练习题中“第一行输入命令数量 Q”的批处理模式，因此测试输入不应包含 Q。

### 支持的命令

| 命令 | 功能 |
|---|---|
| ADD_S id base value threshold | 添加传感器任务 |
| ADD_C id base urgency | 添加控制任务 |
| FIND id | 查询任务 |
| CANCEL id | 取消等待任务 |
| PEEK | 查看堆顶有效任务 |
| RUN | 执行最高优先级任务 |
| LIST k | 查看前 k 个等待任务 |
| IDS left right | 按字典序范围查询 |
| UNDO | 撤销最近一次成功执行 |
| HISTORY k | 查询执行历史 |
| STATS | 输出任务状态统计 |
| SNAPSHOT name | 创建具名快照 |
| RESTORE name | 恢复指定快照 |
| SAVE filename | 保存系统状态 |
| LOAD filename | 加载系统状态 |
| QUIT | 退出程序 |

### 简单运行示例

输入：

```text
ADD_S temp 4 13 10
ADD_C motor 2 3
PEEK
RUN
STATS
QUIT
```

预期输出：

```text
PEEK motor 8
RUN motor 8
STATS 1 1 0
```

## 6. 测试（Testing）

计划使用 `examples/` 保存输入文件及预期输出文件。

目前设计三组回归测试：

- `basic`：任务调度、优先级排序、撤销、历史查询及状态统计。
- `edge_cases`：空队列、重复 ID、非法参数、取消任务及惰性删除。
- `snapshot`：快照、文件保存、加载及状态恢复。

编译后可以使用以下方式对比输出：

```bash
./edgeos < examples/basic.in > actual.out
diff -u examples/basic.expected actual.out
```

若无差异，`diff` 不输出内容。

测试用例是根据 v1.0 源码准备的预期结果，目前尚未完成全部实际编译运行验证。

后续计划加入单元测试、CTest、AddressSanitizer、UndefinedBehaviorSanitizer 以及随机化测试。

## 7. 已知问题（Known Limitations）

当前版本仍存在以下需要完善的方面：

1. 部分跨索引操作的异常安全性仍需加强。
2. AVL 独立复制时，其非拥有型任务指针需要更清晰的生命周期约束。
3. 尚未建立独立的自动化测试、CI 及项目专用 CMake 构建配置。
4. 当前程序仅为调度模拟，不支持真实的并发任务执行。

## 8. 后续开发计划（Roadmap）

### v1.0.1：工程化改进

- [ ] 引入 CMake 和 CTest。
- [ ] 为二叉堆、AVL、散列表和调度器编写单元测试。
- [ ] 添加 Sanitizer 检测。
- [ ] 改进异常安全及资源管理。
- [ ] 统一命名风格与公开接口规范。

### v1.1：动态任务调度

- [ ] 添加运行时任务优先级更新。
- [ ] 支持传感器数据更新后重新计算优先级。
- [ ] 优化二叉堆中指定任务的定位及优先级调整。
- [ ] 引入模拟时钟与调度统计。

### 长期研究方向

在当前模拟系统基础上，探索周期任务、截止时间调度、固定容量任务池、嵌入式资源约束，以及与 FreeRTOS 等实时操作系统调度机制的比较。

这些属于未来研究方向，并非当前已经实现的功能。


## 9. 项目说明

本项目为个人 C++ 与数据结构综合实践项目，主要用于巩固所学知识、积累工程经验并接受代码审查。

EdgeOS 当前定位为教学性质的任务调度模拟器，不是生产环境可用的操作系统或实时调度内核。
