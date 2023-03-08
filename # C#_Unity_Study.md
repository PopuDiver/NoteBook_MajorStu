# # C#_Unity_Study

```c#
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Test
{
	void Main(){
		GameManager.GetInstance();
	}
}

public class BaseManage<T> where T:new()//添加约束 意思是要给T一个有无参函数/new
{
	private static T instance;

	public static T GetInstance()
	{
		if(instance == null){
			instance = new T();
		}
		return instance;
	}
}

public class GameManager : BaseManager<GameManager>
{
	
}



//继承了MonoBehaviour的单例模式基类
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

//继承了 MonoBehaviour 的 单例模式对象 需要我们自己保证它的位移性
public class SingletonMono<T> : MonoBehaviour where T : MonoBehaviour
{
	private static T instance;

	public static T GetInstance()
	{
		//只继承了Mono的脚本 不能够直接new
		//只能通过拖动到对象上 或者 通过 加脚本的API AddComponent去加脚本
		//U3D内部帮助我们去实例化它
		return instance;

	}

	protected virtual void Awake()
	{
		instance = this as T;
	}
}


public class NewBehaviourScript : SingletonMono<NewBehaviourScript>{
	protected override void Awake(){
		base.Awake();
	}
}


//优化继承MonoBehaviour的单例模式
//继承这种自动创建的 单例模式基类 不需要我们手动去拖 或者 API去加载脚本
//想用 直接 GetInstance就可以
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SingletonMono<T> : MonoBehaviour where T : MonoBehaviour
{
	private static T instance;

	public static T GetInstance()
	{
		if( instance == null){
			//让这个单例模式对象 过场景 不移除
			//因为 单例模式对象 往往 是存在整个程序生命周期中的
			DontDestroyOnLoad(obj);

			instance = obj.AddComponent;
		}
		return instance;

	}

}


//缓存池模块

public Dictionary<string, List<GameObject> poolDic = new Dictionary<string , List<GameObject>>();

//往外拿东西
public GameObject GetObj(string name){
	GameObject obj = null;
	//有抽屉 并且抽屉里有东西
	if(poolDic.ContainsKey(name) && poolDic[name].Count > 0){
		obj = poolDic[name][0];
		poolDic[name].RemoveAt(0);
	}else{
		obj = GameObject.Instantiate(Resources.Load<GameObject>(name));
	}
	//激活
	obj.SetActive(ture);
	return obj;
}

//还暂时不用的东西给抽屉
public void PushObj(string name,GameObject obj){
	//失活
	obj.SetActive(false);
	if(poolDic.ContainsKey(name)){
		poolDic[name].Add(obj);
	}else{
		poolDic.Add(name,new List<GameObject>(){ obj });
	}
}
```

C#缓存池模式是为了缓解内存压力 为了减少GC的自动调用 从而解决的一个优化方式 可以手动实现对象的激活和失活 从而减少一直生成和销毁的事件发生。
