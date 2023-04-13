# 1. 线程的创建

## 线程介绍篇

1. 线程的介绍：
   1. 线程是操作系统调度的最小单位。线程包含在进程中，是进程中实际运行的单位。一个进程中可以同时运行多个线程，每个线程可以执行不同的任务，这就是所谓的多线程。
   2. 内核(Kernel)，又称核心，是操作系统最基本的部分，主要负责管理系统资源。
2. 线程的分类：
   1. 用户线程
      1. 用户线程受内核支持，但无须内核管理
      2. 由进程利用线程库创建，释放和管理，因此内核对用户线程不可知，内核只会负责用户线程的进程以及轻量级进程的调度，而不会消耗其他内核资源。因为上下文切换不需要内核干预，所以可以省去用户线程进出内核态的消耗由于用户线程在用户空间中实现，所以一个用户线程如果阻塞在系统调用中，则整个进程都将会阻塞。
   2. 内核线程
      1. 由操作系统直接支持，管理和调度，通过系统调用完成。

## 创建线程

CreateThread()，需要传入六个参数;

参数一是指向security attributes型态的结构的指针，Windows 98中忽略该参数,被置空。

参数二是用于新线程的初始堆栈大小，默认值为0，在任何情况下，Windows根据需要动态延长堆栈的大小

参数三是指向线程函数的指标，函数名称没有限制，但是必须以下列形式声明：DWORD WINAPI ThreadProc(PVOID pParam);

参数四为传递给ThreadProc的参数，这样主线程和从属线程就可以共享数据。

参数五通常为0，但当建立的线程不马上执行时为标记create_suspended，线程将暂停直到呼叫resumeThread来恢复线程的执行为止。

参数六是一个指标，指向接受执行绪ID值的变量。

```c++
#include<iostream>
#include<windows.h>
using namespace std;

//回调函数
DWORD WINAPI ThreadProc(PVOID lp){
    
}

int main(){
    
    //Win API
    CreateThread(NULL,0,ThreadProc,0,0,0);
    
    return 0;
}
```

## 终止线程

1. 线程函数返回
   1. 这是确保线程的所有资源被正确地清除的唯一办法。当线程函数返回时，如下情况将会发生：
      1. 在线程函数中创建的所有C++对象将通过它们的析构函数正确地撤销；
      2. 操作系统将正确地释放线程的堆栈使用的内存；
      3. 系统将线程的退出代码设置为线程函数的返回值；
      4. 系统递减线程内核对象的引用计数。
2. ExitThread函数。
   1. 可以通过在线程中调用ExitThread函数，来强制终止自身线程的运行。原型为：VOID ExitThread(DWORD dwExitCode);
   2. 该函数将终止自身线程的运行，并导致操作系统清除该线程使用的所有操作系统资源。但是，C++资源（如C++对象）将不被正确地撤销。

## 线程的挂起与恢复运行

1. 任何线程都可以调用SuspendThread()来暂停另一个线程的运行(只要拥有线程的句柄。原型为：DWORD SuspendThread(HANDLE hThread（一个句柄）);)
2. 返回值是前一次暂停计数，一个线程能够被暂停的最多次数是MAXIMUM SUSPEND COUNT，
3. 参数HANDLE hThread表示将要被挂起的线程
4. 调用ResumeThread（可以让挂起的线程恢复运行。原型为：DWORD ResumeThread(HANDLE hThread);）
5. 返回值是前一次暂停计数，参数HANDLE hThread表示将要被恢复的线程。

```c++
#include<iostream>
#include<windows.h>
using namespace std;

//回调函数
DWORD WINAPI ThreadProc(PVOID lp){
    
}

int main(){
    //如果进程同时运行，有定时器作为时间片的操作标准 
    //Win API
    HANDLE h = CreateThread(NULL,0,ThreadProc,0,CREATE_SUSPENDED,0);//句柄-内核对象
    //线程的挂起和恢复
    SuspendThread(h);//手动挂起
    ResumeThread(h);//恢复线程 引用计数
    
    return 0;
}
```

## 线程的优先级

七个优先级，可以使用设置优先级函数设置优先级

![image-20230324111011803](https://cdn.jsdelivr.net/gh/PopuDiver/Typora-MapDepot/Typora-MapDepot202303241110973.png)

# 2. 线程间的同步

## 线程等待

```c
#include<stdio.h>
#include<windows.h>

//回调函数
DWORD WINAPI ThreadProc(PVOID lp){
    
}

int main(){
    //如果进程同时运行，有定时器作为时间片的操作标准 
    //Win API
    HANDLE h = CreateThread(NULL,0,ThreadProc,0,0,0);//句柄-内核对象
    //等待
    Sleep(10000);//一般不使用
    WaitForSingleObject(h,INFINITE);//参数一传入句柄，参数二传入时间，此处为无限制等待
    CloseHandle(h);//关闭进程 
    return 0;
}
```

## 线程间的同步-原子锁

1. 线程间的同步
   1. 当多个线程无限制的在同一段时间内访问同一资源时，会发生错误。
2. 线程间同步方法
   1. 原子访问，互锁函数
      1. 互锁函数提供了一套多个线程同步访问一个简单变量的处理机制。
      2. LONG InterlockedIncrement(LONG volatile* IpAddend);该函数提供多线程情况下，对一个变量以原子操作方式增加1
      3. LONG InterlockedDecrement(LONG volatile* IpAddend);该函数提供多线程情况下，对一个变量以原子操作方式减少1
      4. LONG InterlockedExchange(LONG volatile* IpAddend,LONG IValue);该函数提供多线程情况下，对一个变量以原子操作方式用IValue给Ip Target指向的目标变量赋值，并返回赋值以前的Ip Target指向的值
      5. LONG InterlockedExchangeAdd(LONG volatile* IpAddend,LONG IValue);该函数提供多线程情况下，对一个变量以原子操作方式将IpAddend指向的变量增加Value，并返回调用前的IIpAddend指向的目标变量的值
   2. 临界区
      1. 临界区是一段连续的代码区域，它要求在执行前获得对某些共享数据的独占的访问权。如果一个进程中的所有线程中访问这些共享数据的代码都放在临界区中，就能够实现对该共享数据的同步访问。临界区只能用于同步单个进程中的线程。
      2. CRITICAL_SECTION g_sec;实例化临界区对象
      3. InitializeCriticalSection(&g_sec)初始化临界区对象
      4. 进入临界区，临界区对象的引用计数加一，同一个线程可以多次调用EnterCriticalSection，但是如果调用n次EnterCriticalSection以后，必须再调用n次的LeaveCriticalSection，使临界区对象的引用计数变为0
      5. 其它的线程才能进入临界区EnterCriticalSection(&g_sec);
      6. 离开临界区LeaveCriticalSection(&g_sec);
      7. 释放临界区对象DeleteCriticalSection(&g_sec);
   3. 内核对象（用的比较多）
      1. 临界区非常适合于在同一个进程内部以序列化的方式访问共享的数据。然而，有时用户希望一个线程与其它线程执行的某些操作取得同步，这就需要使用内核对象来同步线程。常用的内核对象有：互斥变量、信号量和事件，其他的还包括文件、控制台输入、文件变化通知、可等待的计时器。
      2. 每个内核对象在任何时候都处于两种状态之一：信号态(signaled)和无信号态(nonsignaled)。
      3. 线程在等待其中的一个或多个内核对象时，如果在等待的一个或多个内核对象处于无信号态，线程自身将被系统挂起，直到等待的内核对象变为有信号状态时，线程才恢复运行。常用的等待函数有两个：
         1. 等待线程函数：在多线程下面，有时候我们会希望等待某一线程完成了再继续做其它事情，要实现这个目的，可以使用Windows API函数WaitForSingleObject，或者WaitForMultipleObjects。这两个函数都会等待Object被标为有信号(signaled)时才返回的。这两个函数的优点是它们在等待的过程中会进入一个非常高效沉睡状态，只占用极少的CPU时间片。
            1. WaitForSingleObject():
               1. 格式 DWORD WaitForSingleObject(HAND hHandle,DWORD dwMilliseconds);有两个参数，分别是THandle和Timeout(毫秒单位)。如果想要等待一条线程，那么你需要指定线程的Handle，以及相应的Timeout时间。当然，如果你想无限等待下去，Timeout参数可以指定系统常量INFINITE。
               2. 使用对象：Event,Mutex,Semaphore,Process,Thread
               3. 返回类型：WAIT OBJECT 0,表示等待的对象有信号（对线程来说，表示执行结束）;WAIT TIMEOUT，表示等待指定时间内，对象一直没有信号（线程没执行完）；WAIT ABANDONED表示对象有信号，但还是不能执行，一般是因为未获取到锁或其它原因。
            2. WaitForMultipleObjects():
               1. 格式为DWORD WaitForMultipleObjects(DWORD nCount,CONST HANDLE *IpHandles,BOOLfWaitAll,DWORDdwMilliseconds);四个参数分别为：参数一DWORD类型，用于指定句柄数组的数量、参数二Pointer类型，用于指定句柄数组的内存地址、参数三Boolean类型，True表示函数等待所有指定句柄的Object有信号为止、参数四DWORD类型，用于指定等待的Timeout时间，单位毫秒，可以是INFINITE。
               2. 当WaitForMultipleObjects等待多个内核对象的时候，如果它的bWaitAll参数设置为false。其返回值减去WAIT OBJECT 0就是参数IpHandles数组的序号。如果同时有多个内核对象被触发，这个函数返回的只是其中序号最小的那个。
               3. 如果为TRUE，则等待所有信号量有效再往下执行（FALSE 当有其中一个信号量有效时就向下执行）
      4. 互斥变量
         1. 互斥变量类似于临界区，但它能够同步多个进程间的数据访问。
         2. CreateMutex(NULL,FALSE,"MutexForSubThread");//创建互斥量
         3. WaitForSingleObject(g_hMutex,INFINITE);//等待互斥量，如果互斥量处于信号态，该函数返回，同时将互斥量自动变为无信号态
         4. 访问共享数据区
         5. ReleaseMutex(g hMutex);使互斥量重新处于信号态
      5. 信号量
         1. 信号量内核对象用于资源计数。每当线程调用WaitForSingleObject()函数并传入一个信号量对象的句柄，系统将检查该信号量的资源计数是否大于0 。如果大于0，表示有资源可用，此时系统就将资源计数减去1，并唤醒线程；如果等于0，表示无资源可用，系统就将线程挂起，直到另外一个线程释放了该对象，释放信号量意味着增加它的资源计数。
         2. 信号量与临界区和互斥量不同，信号量不属于任何线程，因此可以在一个线程中增加信号量的计数，而在另一个线程中减少信号量的计数。
         3. 但是在使用过程中，信号量的使用与互斥量非常相似，互斥量可以看作是信号量的一个特殊版本，即可以将互斥量看作最大资源计数为1的信号量。
         4. 信号量常用在如下情况下，M个线程对N个共享资源的访问，其中M > N.
            1. CreateSemaphore(NULL,iCount,_T ("Semaphore"));//创建信号量
            2. WaitForSingleObject(m hSemaphore,INFINITE);//等待信号量
            3. ReleaseSemaphore(m hSemaphore,1,NULL);//释放信号量
      6. 事件对象
         1. 与互斥量和信号量不同，互斥变量和信号量用于控制对共享数据的访问，而事件发送信号表示某一操作已经完成。
         2. 有两种事件对象：手动重置事件和自动重置事件。
            1. 手动重置事件用于同时向多个线程发送信号；自动重置事件用于向一个线程发送信号。
            2. 如果有多个线程调用WaitForSingleObject()或者WaitForMultipleObjects()等待一个自动重置事件，那么当该自动重置事件变为信号态时，其中的一个线程会被唤醒，被唤醒的线程开始继续运行，同时自动重置事件又被置为无信号态，其它线程依旧处于挂起状态。从这一点看，自动重置事件有点类似于互斥量
            3. 手动重置事件不会被WaitForSingleObject()和WaitForMultipleObjects()自动重置为无信号态，需要调用相应的函数才能将手动重置事件重置为无信号态。因此，当手动重置事件有信号时，所有等待该事件的线程都将被激活。
            4. 例如对于进程或线程对象，等待成功就表示进程或线程执行结束了；对于互斥量对象，则表示此对象现在不被任何其它线程拥有，并且一旦等待成功，当前线程即拥有了此互斥量，其它线程则不能同时拥有，直接调用ReleaseMutex函数主动释放互斥量。
         3. 使用CreateEvent()函数创建事件对象：
            1. HANDLE CreateEvent(LPSECURITY ATTRIBUTES IpEventAttributes,BOOL bManualReset,BOOL bInitialState,LPCTSTR IpName);
            2. bManualReset为true时，指定创建的是手动重置事件，FALSE当一个等待线程被释放时，自动重置状态为无信号状态；
            3. bInitialState表示事件对象被初始化时是信号态还是无信号态；
            4. 参数IpName指定事件对象的名称，其它进程中的线程可以通过该名称调用CreateEvent()或者OpenEvent()函数得到该事件对象的句柄。
         4. 通过SetEvent()函数设置为信号态：
            1. 注意：无论自动重置事件对象还是手动重置事件对象，都可以设置为有信号状态BOOL SetEvent(HANDLE hEvent);
         5. 通过ResetEvent函数设置为无信号态：
            1. 注意：无论自动重置事件对象还是手动重置事件对象，都可以BOOL ResetEvent(HANDLE hEvent);
            2. 不过对于自动重置事件不必执行ResetEvent，因为系统会在WaitForSingleObject()或者WaitForMultipleObjects()返回前，自动将事件对象置为无信号态。
         6. 调用CloseHandle(),函数关闭事件

```c
//原子锁代码
#include<stdio.h>
#include<windows.h>

unsigned int g;
//回调函数
DWORD WINAPI ThreadProc(PVOID lp){
    for(int i = 0 ;i < 10000;i++){
        //临界资源（临界区）
        InterlockedIncreemnt(&g);//以原子方式自动加一
    }
}

int main(){
    //如果进程同时运行，有定时器作为时间片的操作标准 
    //Win API
    HANDLE h = CreateThread(NULL,0,ThreadProc,0,0,0);//句柄-内核对象
    for(int i = 0;i < 100000;i++){
        InterlockedIncrement(&g);
    }
    //等待
    Sleep(10000);//一般不使用
    WaitForSingleObject(h,INFINITE);//参数一传入句柄，参数二传入时间，此处为无限制等待
    CloseHandle(h);//关闭进程 
    return 0;
}
```

```c
//临界区代码
#include<stdio.h>
#include<windows.h>

unsigned int g;

//创建临界区对象
CRITICAL_SECTION g_sec;

//回调函数
DWORD WINAPI ThreadProc(PVOID lp){
    for(int i = 0 ;i < 10000;i++){
        //临界资源（临界区）
        EnterCriticalSection(&g_sec);
        g++;
        LeaveCriticalSection(&g_sec);
    }
}

int main(){
    //初始化
    InitializeCriticalSection(&g_sec);
    
    //如果进程同时运行，有定时器作为时间片的操作标准 
    //Win API
    HANDLE h = CreateThread(NULL,0,ThreadProc,0,0,0);//句柄-内核对象
    for(int i = 0;i < 100000;i++){
        //临界资源（临界区）
        EnterCriticalSection(&g_sec);
        g++;
        LeaveCriticalSection(&g_sec);
    }
    //等待
    //Sleep(10000);//一般不使用
    WaitForSingleObject(h,INFINITE);//参数一传入句柄，参数二传入时间，此处为无限制等待
    //释放
    CloseHandle(h);//关闭进程 
    DeleteCriticalSection(&g_sec);
    return 0;
}
```

```c
//内核对象代码
#include<stdio.h>
#include<windows.h>

unsigned int g;

HANDLE hmutex;

//回调函数
DWORD WINAPI ThreadProc(PVOID lp){
    for(int i = 0 ;i < 10000;i++){
        WaitForSingleObject(hmutex,INFINITE);//置为无信号
        g++;//处理数据
        RealeaseMutex(hmutex);//置为有信号
    }
}

int main(){
   	//创建内核对象
    hmutex = CreateMutex(NULL,FALSE,NULL);
    //如果进程同时运行，有定时器作为时间片的操作标准 
    //Win API
    HANDLE h = CreateThread(NULL,0,ThreadProc,0,0,0);//句柄-内核对象
    for(int i = 0;i < 100000;i++){
        WaitForSingleObject(hmutex,INFINITE);//置为无信号
        g++;//处理数据
        RealeaseMutex(hmutex);//置为有信号
    }
    //等待
    //Sleep(10000);//一般不使用
    WaitForSingleObject(h,INFINITE);//参数一传入句柄，参数二传入时间，此处为无限制等待
    //释放
    CloseHandle(h);//关闭进程 
    CloseHandle(hmutex);
    DeleteCriticalSection(&g_sec);
    return 0;
}
```

```c
//事件对象代码
#include<stdio.h>
#include<windows.h>

unsigned int g;

//创建事件对象
HANDLE hEvent;

//回调函数
DWORD WINAPI ThreadProc(PVOID lp){
    for(int i = 0 ;i < 10000;i++){
        WaitForSingleObject(hEvent,INFINITE);//等待事件
        g++;//处理数据
        SetEvent(hEvent);//自动重置事件
    }
}

int main(){
   	
    //创建事件对象
    hEvent = CreateEvent(NULL,FALSE,TRUE,NULL);
    
    //如果进程同时运行，有定时器作为时间片的操作标准 
    //Win API
    HANDLE h = CreateThread(NULL,0,ThreadProc,0,0,0);//句柄-内核对象
    for(int i = 0;i < 100000;i++){
        WaitForSingleObject(hEvent,INFINITE);//等待事件
        g++;//处理数据
        SetEvent(hEvent);//自动重置事件
    }
    
    //等待
    WaitForSingleObject(h,INFINITE);//参数一传入句柄，参数二传入时间，此处为无限制等待
    //释放
    CloseHandle(h);//关闭进程 
    CloseHandle(hmutex);
    DeleteCriticalSection(&g_sec);
    return 0;
}
```

## 线程的死锁

假如有两个线程，一个线程想先锁对象1，再锁对象2，恰好另外有一个线程先锁对象2，再锁对象1。在这个过程中，当线程1把对象1锁好以后，就想去锁对象2，但是不巧，线程2已经把对象2锁上了，也正在尝试去锁对象1.什么时候结束呢，只有线程1把两个对象都锁上并把方法执行完，并且线程2把两个对象也都锁上并且把方法执行完毕，那么就结束了，但是，谁都不肯放掉已经锁上的对象，所以就没有结果，这种情况就叫做线程死锁。多个线程间如果相互等待对方拥有的资源，将可能发生死锁。

## 线程间通信

1. 使用全局变量进行通信
2. 参数传递方式
3. 消息传递方式

# 进程的创建

1. 进程的介绍：
   1. 狭义定义：进程是正在运行的程序的实例
   2. 广义定义：进程是一个具有一定独立功能的程序关于某个数据集合的一次运行活动。它是操作系统动态执行的基本单元，在传统的操作系统中，进程既是基本的分配单元，也是基本的执行单元。
   3. 进程的基本状态：![image-20230323101135475](https://cdn.jsdelivr.net/gh/PopuDiver/Typora-MapDepot/Typora-MapDepot202303231011553.png)
   4. 创建状态：进程在创建时需要申请一个空白PCB(进程控制块)，向其中填写控制和管理进程的信息，完成资源分配。如果创建工作无法完成，比如资源无法满足，就无法被调度运行，把此时进程所处状态称为创建状态。
   5. 就绪状态：进程已经准备好，已分配到所需资源，只要分配到CPU就能够立即运行
   6. 执行状态：进程处于就绪状态被调度后，进程进入执行状态
   7. 阻塞状态：正在执行的进程由于某些事件（I/O请求，申请缓存区失败）而暂时无法运行，进程受到阻塞。在满足请求时进入就绪状态等待系统调用
   8. 终止状态：进程结束，或出现错误，或被系统终止，进入终止状态。无法再执行。
   
2. 进程的同步与互斥
   1. 同步：为了完成某个任务而建立的两个或多个进程，这些进程为了需要在某些位置上协调它们的工作次序而等待、传递信息所产生的制约关系。进程间的制约关系就是源于它们之间的相互合作。
   2. 互斥：当一个进程进入临界区使用临界资源时，另一个进程必须等待，当占用临界区资源的进程退出临界区以后才允许进入临界区，访问资源。注意，这里的一个并非规定就一次只能一个进程访问临界资源。当信号量个数大于1的时候，一次可以有多个进程同时访问临界资源。
   3. 同步准则：
      1. 空闲让进：临界区（每个进程中访问临界资源的那段代码称为临界区）空闲时，应该允许某个进程立即进入临界区访问临界资源。
      2. 忙则等待：当临界区进程访问数达到访问上限时，其它试图进入临界区的进程应该等待。
      3. 有限等待：应该避免进程一直等待永远进入不了临界区的情况。
      4. 让权等待：“权”即CPU使用权，当进程不能进入临界区的时候，应该放弃使用CPU，进入等待队列，而不是“忙等待”。
   
3. 进程的创建

   1. ```c
      //进程创建代码
      #include<stdio.h>
      using namespace std;
      
      int main(){
          TCHAR commandline[] = TEXT("路径");
          _STARTUPINFOW startinfo = {sizeof(_STARTUPINFOW)};
          _PROCESS_INFORMATION processInfo;
          //创建进程
          bool ret = CreateProcess(
          	NULL,//不指定可执行文件的文件名
              commandline,//命令行参数
              NULL,//默认进程安全性
              NULL,//默认线程安全性
              FALSE,//指定当前进程的句柄是否被子进程继承
              0,//指定附加的、用来控制优先类和进程的创建的标志
              NULL,//使用当前进程的环境变量
              NULL,//使用当前进程的驱动和目录
              &startinfo,//指向一个用于决定新进程的主窗体如何显示的STARTUPINE0的结构体
              &processInfo//进程信息结构体
          );
          if(!ret){
              cout << "进程创建失败" << endl;
              return 0;
          }else{
              WaitForSingleObject(precessInfo.hProcess,INFINITE);
              printf("新创建的进程ID：%d\n",process.dwProcessId);
              printf("新创建的进程ID：%d",process.dwThreadId);
              CloseHandle(processInfo.hProcess);
              CloseHandle(processInfo.hThread);
          }
          return 0;
      }
      ```


# 进程间通信

1. 无名管道(pipe)：管道是一种半双工的通信方式，数据只能单向流动，而且只能在具有亲缘关系的进程间使用。进程的亲缘关系通常是指父子进程关系。

2. 命名管道(named pipe)：有名管道也是半双工的通信方式，但是它允许无亲缘关系进程间的通信。

3. 消息队列(message queue)：消息队列是由消息的链表，存放在内核中并由消息队列标识符标识。消息队列克服了信号传递信息少、管道只能承载无格式字节流以及缓冲区大小受限等缺点。

4. 互斥量

5. 共享内存(shared memory)：共享内存就是映射一段能被其他进程所访问的内存，这段共享内存由一个进程创建，但多个进程都可以访问。共享内存是最快的IPC方式，它是针对其他进程间通信方式运行效率低而专门设计的。它往往与其他通信机制，如信号量，配合使用，来实现进程间的同步和通信。

6. 方法一：WM_COPYDATA (WM是计算机自带的宏定义)

7. 方法二：剪切板

8. 方法三：共享文件映射（最快的

   1. ```c
      #include<stdio.h>
      #include<Windows.h>
      
      int main(){
          //创建共享文件句柄
          /*HANDLE hMapFile = CreateFileMapping(
          		INVALID HANDLE VALUE,//物理文件句柄
              	NULL,//默认安全级别
              	PAGE_READWRITE,//可读可写
              	0,//高位文件大小
              	BUF_SIZE,//低位文件大小
              	szName//共享内存名称
          );*/
          HANDLE hMapFile = CreateFileMapping(
          		NULL,
              	NULL,
              	PAGE_READWRITE,
              	0,
              	128,
              	TEXT("文件名")
          );
          
          char* buf = (char*)MapViewOfFile(
              	hMapFile,//句柄
              	FILE_MAP_ALL_ACCESS,//读写权限
              	0,//偏移量
              	0,//高位
              	128//低位
          );
          
          //读数据
          gets_s(buf,100);
          
          getchar();
          UnmapViewOfFile(buf);
          CloseHandle(hMapFile);
          
          return 0;
      }
      ```

## 套接字（socket）

套接字也是进程间通信机制，与其他通信机制不同的是，它可用于不同机器间的进程通信。

# 内存管理

1. 进程的虚拟地址空间
   1. 对于32位进程来说，这个地址空间是4GB，因为32位指针可以拥有从0x00000000至0xFFFFFFFF之间的任何一个值。这使得一个指针能够拥有4294967296个值中的一个值，它覆盖了一个进程的4GB虚拟空间的范围。对于64位进程来说，这个地址空间是16EB（1TB = 1024GB，1PB = 1024TB ,1EB = 1024PB）,因为64位指针可以拥有从0x0000000000000000至0xFFFFFFFFFFFFFFFF之间的任何值，这使得一个指针可以拥有18446744073709551616个值中的一个值，它覆盖了一个进程的16EB虚拟空间的范围。这是相当大的一个范围。
   2. 32位的CPU的寻址空间是4G，所以虚拟内存的最大值为4G，而Windows操作系统把这4G分成2部分，即3G的用户空间和1G的系统空间，系统空间是各个进程所共享的，他存放的是操作系统及一些内核对象等，而用户空间是分配给各个进程使用的，用户空间包括用：程序代码和数据，堆，共享库，栈。
2. 分页
   1. 分页的基本方法是，将地址空间分成许多的页，每页的大小由CPU决定，然后由操作系统选择页的大小。目前Inter系列的CPU支持4KB或4MB的页大小，而PC上目前都选择使用4KB.

#  网络的相关概念

## 基本概念

网络：网络是各种连在一起的可以相互通信的设备的集合

速率：网络技术中的速率指的是连接在计算机网络上的主机在数字信道上传送数据的速率，它也被称为数据率或比特率。速率是计算机网络中最重要的一个性能指标。

## 网络组成

计算机网络是由不同通信媒体和中继设备连接的、物理上独立的多台计算机组成的、将需传输的数据分成不同长度的分组进行传输和处理的系统多台自主计算机的互联系统必须指定网络中哪一台计算机来完成什么样的操作，即客户/服务器的工作方式。

![image-20230331113827829](https://cdn.jsdelivr.net/gh/PopuDiver/Typora-MapDepot/Typora-MapDepot202303311138975.png)

## TCP协议

三次握手建立连接：

1.第一次握手：建立连接。客户端发送连接请求报文段，将SYN位置为1，Sequence Number为x；然后，客户端进入SYN_SENT 状态，等待服务器的确认。

2.第二次握手：服务器收到SYN报文段。服务器收到客户端的SYN报文段，需要对这个SYN报文段进行确认，设置Acknowledgment Number为x + 1；同时，自己还要发送SYN请求信息，将SYN位置为1，Sequence Number为y，服务器将上述所有信息放到一个报文段（即SYN + ACK报文段）中，一并发送给客户端，此时服务器进入SYN_RECV状态。

3.第三次握手：客户端收到服务器的SYN + ACK报文段。然后将Acknowledgment Number设置为y + 1，向服务器发送ACK报文段，这个报文段发送完毕以后，客户端和服务端都进入ESTABLISHED状态，完成TCP三次握手。可以开始传输数据。

四次挥手断开连接：

当客户端和服务器通过三次握手建立了TCP连接以后，当数据传送完毕，断开TCP连接。

1.第一次挥手：主机1（可以是客户端，也可以是服务器端），设置Sequence Number和Acknowledgment Number，向主机2发送一个FIN报文段；此时，主机1进入FIN_WAIT_1状态；这表示主机1没有数据要发送给主机2了；

2.第二次挥手：主机2收到了主机1发送的FIN报文段，向主机1回一个ACK报文段，Acknowledgment Number为Sequence Number加一；主机1进入FIN_WAIT_2状态；主机2告诉主机1，我也没有数据要发送了，可以关闭连接了；

3.第三次挥手：主机2向主机1发送FIN报文段，请求关闭连接，同时主机2进 入CLOSE_WAIT状态；

4.第四次挥手：主机1收到主机2发送的FIN报文段，向主机2发送ACK报文段，然后主机1进入TIME_WAIT状态；主机2收到主机1的ACK报文段以后，就关闭连接；此时，主机1等待2MSL后依然没有收到回复，则证明Server端已正常关闭，主机1也可以关闭连接。

## UDP协议

UDP是一个简单的面向数据报的运输层协议：进程的每个输出操作都正好产生一个UDP数据报，并组装成一份待发送的IP数据报。这与面向流字符的协议不同，如TCP，应用程序产生的全体数据与真正发送的单个IP数据报可能没有什么联系。

特点：

1. 是无连接的。相比于TCP协议，UDP协议在传送数据前不需要建立连接，当然也就没有释放连接。
2. 是尽最大努力交付的。也就是说UDP协议无法保证数据能够准确的交付到目的主机。也不需要对接收到的UDP报文进行确认。
3. 是面向报文的。也就是说UDP协议将应用层传输下来的数据封装在一个UDP包中，不进行拆分或合并。因此，运输层在收到对方的UDP包后，会去掉首部后，将数据原封不动的交给应用进程。
4. 没有拥塞控制。因此UDP协议的发送速率不受网络的拥塞度影响。
5. UDP支持一对一、一对多、多对一和多对多的交互通信。
6. UDP的头部占用较小，只占用8个字节。

## TCP和UDP的区别

TCP是传输控制协议，提供的是面向连接、可靠的字节流服务。当客户和服务器彼此交换数据前，必须先在双方之间建立一个TCP连接，之后才能传输数据。TCP提供超时重发，丢弃重复数据，检验数据，流量控制等功能，保证数据能从一端传到另一端。

UDP是用户数据报协议，是一个简单的面向数据报的运输层协议。UDP不提供可靠性，它只是把应用程序传给IP层的数据报发送出去，但是并不能保证它们能到达目的地。由于UDP在传输数据报前不用在客户和服务器之间建立一个连接，且没有超时重发等机制，故而传输速度很快。

# 套接字（Scoket）

## 概述

Scoket(套接字)可以看成是两个网络应用程序进行通信时，各自通信连接中的端点，这是一个逻辑上的概念。

进程从网络中接收数据，向网络发送数据都是通过套接字来进行的。

scoket可以看成是==程序访问系统网络组件的接口。==