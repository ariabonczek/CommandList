//-----------------------------------
// PROPERTY OF ARIA BONCZEK
// COPYRIGHT(c) UNDER THE MIT LICENSE
//-----------------------------------

#include <tuple>
#include <iostream>

/// <summary>
/// Sample to prove it works with user-defined types
/// </summary>
struct Vector3
{
	float x, y, z;
	Vector3() {}
};

template<int ...>struct seq{};
template<int N, int ...S> struct gens : gens<N - 1, N - 1, S...> {};
template<int ...S>struct gens<0, S...> { typedef seq<S...> type; };

/// <summary>
/// A generic command list that can store functions with any number of 
/// </summary>
template <typename returnType, typename ... types>
class CommandList
{
public:
	typedef returnType(*commandptr)(types...);

	CommandList(int reserve = 10):
		reserveCount(reserve),
		numCommands(0)
	{
		commands = new commandptr[reserve];
		parameters = new std::tuple<types...>[reserve];
	}

	CommandList(const CommandList& clist)
	{
		numCommands = clist.numCommands;
		commands = clist.numCommands;
		parameters = clist.parameters;
	}

	~CommandList()
	{
		delete[] commands;
		delete[] parameters;
	}

	CommandList& operator=(const CommandList& clist)
	{
		numCommands = clist.numCommands;
		commands = clist.numCommands;
		parameters = clist.parameters;
		return this;
	}

	/// <summary>
	/// Executes the commands, storing their return values in the specified array
	/// </summary>
	void ExecuteList(returnType* returns = 0)
	{
		if (returns)
		{
			ExecuteInternal<returnType>(returns);
		}
		else
		{
			ExecuteInternal<void>(returns);
		}
		numCommands = 0;
	}

	/// <summary>
	/// Resets the numCommands to 0, effectively clearing the list
	/// </summary>
	void ClearList()
	{
		numCommands = 0;
	}

	/// <summary>
	/// Registers the function pointer in the command list. Will resize if necessary
	/// </summary>
	void RegisterCommand(returnType (*_command)(types ...), types... t)
	{
		if (numCommands == reserveCount)
		{
			reserveCount = std::floor(reserveCount * 1.33);

			commandptr* newcommands = new commandptr[reserveCount];
			std::tuple<types...>* newparams = new std::tuple<types...>[reserveCount];

			memcpy(newcommands, commands, sizeof(commandptr) * numCommands);
			memcpy(newparams, parameters, sizeof(std::tuple<types...>)* numCommands);

			delete[] commands;
			delete[] parameters;

			commands = newcommands;
			parameters = newparams;
		}
		commands[numCommands] = _command;
		parameters[numCommands] = std::make_tuple(t...);
		++numCommands;
	}
private:
	/// <summary>
	/// Calls the command at index s
	/// </summary>
	template <int ...S>
	returnType CallFunc(int i, seq<S...>)
	{
		return commands[i](std::get<S>(parameters[i]) ...);
	}

	/// <summary>
	/// Internal function used to call the command
	/// </summary>
	template<typename T>
	void ExecuteInternal(returnType* returns)
	{
		for (unsigned int i = 0; i < numCommands; ++i)
		{
			returns[i] = CallFunc(i, typename gens<sizeof...(types)>::type());
		}
	}

	/// <summary>
	/// Template specialization to avoid trying to set a void
	/// </summary>
	template<>
	void ExecuteInternal<void>(returnType* returns)
	{
		for (unsigned int i = 0; i < numCommands; ++i)
		{
			CallFunc(i, typename gens<sizeof...(types)>::type());
		}
	}

	commandptr* commands;
	std::tuple<types...>* parameters;
	unsigned int numCommands;
	unsigned int reserveCount;
};


void Function1()
{
	std::cout << "Function1" << std::endl;
}

void Function2()
{
	std::cout << "Function2" << std::endl;
}

void Function3()
{
	std::cout << "Function3" << std::endl;
}

int Add(int a, int b)
{
	std::cout << "Add" << std::endl;

	return a + b;
}

Vector3 Func(char c1, char c2, double n, float n1, float n2, short s1, char* string)
{
	std::cout << "Vector3 Func" << std::endl;

	return Vector3();
}

int main()
{
	CommandList<Vector3, char, char, double, float, float, short, char*> list;
	
	list.RegisterCommand(Func, 'x', 'c', 2.3, 2.3f, 2.3f, 2, "hello");

	list.ExecuteList();

	CommandList<void> cmdList;

	cmdList.RegisterCommand(Function1);
	cmdList.RegisterCommand(Function3);
	cmdList.RegisterCommand(Function2);
	cmdList.RegisterCommand(Function1);
	cmdList.RegisterCommand(Function1);
	cmdList.RegisterCommand(Function2);
	cmdList.RegisterCommand(Function1);
	cmdList.RegisterCommand(Function3); cmdList.RegisterCommand(Function1);
	cmdList.RegisterCommand(Function3);
	cmdList.RegisterCommand(Function2);
	cmdList.RegisterCommand(Function1);
	cmdList.RegisterCommand(Function1);
	cmdList.RegisterCommand(Function2);
	cmdList.RegisterCommand(Function1);
	cmdList.RegisterCommand(Function3);

	cmdList.ExecuteList();

	CommandList<int, int, int> addList;

	addList.RegisterCommand(Add, 2, 3);
	addList.RegisterCommand(Add, 5, 6);

	int nums[2] = {};
	addList.ExecuteList(nums);

	system("PAUSE");
}