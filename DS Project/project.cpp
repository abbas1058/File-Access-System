#include <iostream>
#include <vector>
#include <list>
#include <fstream>
using namespace std;
// Heap
class User_Information
{
private:
    int User_id;
    string Operator;
    int priority;
public:
    User_Information()
    {
        User_id = 0;
        priority = 0;
        Operator = "null";
    }
    User_Information(int user, string op, int Priority)
    {
        User_id = user;
        priority = Priority;
        Operator = op;
    }
    friend ostream& operator<<(ostream& out, const User_Information& o)
    {

        cout << " User ID : " << o.User_id << endl;
        cout << " Priority : " << o.priority << endl;
        cout << " Operator : " << o.Operator << endl;

        return out;
    }
    template<class t>
    friend class Max_heap;
    template<class y>
    friend class User;
};
// Priority Queue
template <class T>
class Max_heap
{
private:
    vector<User_Information> array;
    int max_size;
    User_Information minimum;
public:
    Max_heap()
    {
        max_size = 0;
        minimum.priority = 10000;
    }
    bool Is_Empty()
    {
        return array.empty();

    }
    int Array_size()
    {
        return array.size();
    }
    T& getMax()
    {
        return array[0];//O(1)
    }
    void insert(const T& key)
    {
        array.push_back(key);//key becomes the next leaf the last leve
        max_size = array.size(); //index of the newly added node
        if (key.priority < minimum.priority)
        {
            minimum = key; // Getting minimum key in O(1)
        }
        Re_HeapUp(max_size - 1);
    }
    void Re_HeapUp(int last)
    {

        int parent = (last - 1) / 2;
        if (array[parent].priority < array[last].priority)
        {
            swap(array[last], array[parent]);
            Re_HeapUp(parent);
        }
    }
    T Extract_Max()
    {

        User_Information Maximum_Value = array[0];
        max_size = array.size() - 1;
        array[0] = array[max_size];//it is overwriting the root with last leaf
        array.pop_back();//it will remove the last leaf
        max_size = array.size();
        Re_heapDown(0, max_size - 1);
        return Maximum_Value;
    }
    // Function to Re_heap down the minimum value
    void Re_heapDown(int parent, int last)
    {
        int leftChild = (2 * parent) + 1;
        int rightChild = (2 * parent) + 2;
        int maxChild;
        if (leftChild <= last)
        {
            if (leftChild == last)
            {
                maxChild = leftChild; // it works for the one child
            }
            else
            {
                if (array[leftChild].priority <= array[rightChild].priority)
                {
                    maxChild = rightChild;
                }
                else
                {
                    maxChild = leftChild;
                }
            }
            if (array[parent].priority < array[maxChild].priority)
            {
                swap(array[parent], array[maxChild]);
                Re_heapDown(maxChild, last);
            }
        }
    }
    void print()
    {
        cout << "The Information of the File is: " << endl;
        for (int i = 0; i < array.size(); i++)
        {
            cout << array[i] << " " << endl;
        }
    }
    template<class s>
    friend class User;
};

// Class for file ID
class file {
private:
    int f_id;
    vector<User_Information> access;
    Max_heap<User_Information>* waiting_queue;
public:
    file()
    {
        f_id = 0;
        waiting_queue = new Max_heap<User_Information>;
    }
    file(int fId)
    {
        f_id = fId;
        waiting_queue = new Max_heap<User_Information>;
    }
    void print_access()
    {
        for (int i = 0; i < access.size(); i++)
        {
            cout << access[i];
        }
    }

    friend ostream& operator<<(ostream& out, file& o)
    {

        out << "File Id : " << o.f_id << endl;
        o.print_access();
        return out;
    }
    template <class e>
    friend class User;
    friend class User_Information;
};

template<class T>
// Hash table
class User
{
private:
    vector<list<T>> H_Table;
    int T_size;
public:
    User()
    {
        T_size = 101;
        H_Table.resize(T_size);
    }
    int Hashing_Function(int id)
    {
        return id % T_size;
    }
    void insert(T file)
    {
        int a = Hashing_Function(file.f_id);
        H_Table[a].push_front(file);
    }
    void Request_Access(int User_Id, string Op, int priority, int file_Id)
    {
        User_Information object(User_Id, Op, priority);
        typename list<T>::iterator Iter;
        int i = Hashing_Function(file_Id);
        for (Iter = H_Table[i].begin(); Iter != H_Table[i].end() && Iter->f_id != file_Id; Iter++)
        {
        }
        if (Iter == H_Table[i].end())
        {
            cout << "FILE NOT FIND" << endl;
            return;
        }
        else
        {
            // check 1
            if (Iter->waiting_queue->Is_Empty() && Iter->access.empty())
            {
                Iter->access.push_back(object);
            }
            else
            {   // read with other user
                if (Iter->access.size() >= 1 && Iter->access[0].Operator == "read" && Op == "read" && Iter->waiting_queue->Is_Empty())
                {
                    Iter->access.push_back(object);
                }
                else
                {
                    Iter->waiting_queue->insert(object); // If one is writing and other is readig then go to waiting queue
                }
                if (!Iter->waiting_queue->Is_Empty())//waiting queue is not empty (giving access to maximum)
                {
                    User_Information temp = Iter->waiting_queue->getMax(); // will get the maximum and check whether reading
                    if (Iter->access.size() >= 1 && Iter->access[0].Operator == "read" && temp.Operator == "read")
                    {
                        Iter->access.push_back(temp); // temp access
                        Iter->waiting_queue->Extract_Max(); // extract
                    }

                }
            }
        }

    }

    void Release_File(int file_Id, int user_Id)
    {
        typename list<T>::iterator Iter;
        int i = Hashing_Function(file_Id);
        for (Iter = H_Table[i].begin(); Iter != H_Table[i].end(); Iter++)
        {
            if (Iter->f_id == file_Id)
            {
                break;
            }
        }
        if (Iter == H_Table[i].end())
        {
            cout << "FILE NOT FIND" << endl;
            return;
        }
        else
        {
            int j;
            for (j = 0; j < Iter->access.size(); j++)
            {
                if (Iter->access[j].User_id == user_Id)
                {
                    break;
                }
            }
            if (j == Iter->access.size())
            {
                cout << "USER NOT FOUND " << endl;
            }
            else
            {
                Iter->access.erase(Iter->access.begin() + j);
                if (Iter->access.empty() && Iter->waiting_queue->Array_size() != 0) //1- access to the next
                {
                    if (Iter->waiting_queue->getMax().Operator == "write") // acess khali hogai ti waiting q tu frst wala write krne betha
                    {
                        User_Information object = Iter->waiting_queue->Extract_Max();
                        Iter->access.push_back(object);
                    }
                    else //For read
                    {
                        while (Iter->waiting_queue->Is_Empty() != true && Iter->waiting_queue->getMax().Operator == "read")
                        {// accesss se nikalo ....hr koi read arha ,,,,, sbko access me daal do
                            User_Information object = Iter->waiting_queue->Extract_Max();
                            Iter->access.push_back(object);

                        }
                    }
                }
            }
        }

    }// Delete a file no one is accessing
    void Delete_File(int f_id)
    {
        typename list<T>::iterator Iter;
        int i = Hashing_Function(f_id);
        for (Iter = H_Table[i].begin(); Iter != H_Table[i].end(); Iter++)
        {

            if (Iter->f_id == f_id)
            {
                break;
            }
        }
        if (Iter == H_Table[i].end())
        {
            cout << "FILE NOT FIND" << endl;
            return;
        }
        else
        {
            if (Iter->access.empty() && Iter->waiting_queue->Is_Empty())
            {
                H_Table[i].erase(Iter);
            }

        }
    }
    void Print()
    {
        typename list<T>::iterator iter;
        for (int i = 0; i < 101; i++)
        {
            for (iter = H_Table[i].begin(); iter != H_Table[i].end(); iter++)
            {

                cout << "H " << i << "->" << *iter << endl;
                cout << "Next" << iter->waiting_queue->getMax();
                if (iter->waiting_queue->Array_size() == 0)
                {
                    cout << "Waiting None ";
                }
                else
                {
                    cout << "Waiting : " << iter->waiting_queue->Array_size() << endl;
                }
            }
        }
    }
    void Load_Data()
    {
        ifstream f("data.txt");

        char comma;
        string type;
        int f_id, u_id, priority;

        while (!f.eof())
        {
            f >> f_id >> comma >> u_id >> comma >> priority >> comma >> type;
            this->Request_Access(u_id, type, priority, f_id);
        }

        f.close();
    }
    void menu()
    {


        for (int number = 0; number <= 7; number++)
        {
            cout << "************ MENU **************" << endl;
            cout << " Enter 1 Print Hash table" << endl;
            cout << " Enter 2 to Load data" << endl;
            cout << " Enter 3 Insert a file" << endl;
            cout << " Enter 4 Delete a file" << endl;
            cout << " Enter 5 Request access to a file by a user" << endl;
            cout << " Enter 6 Release the file by a user" << endl;
            cout << " Enter 7 Exit " << endl;

            cout << "Enter Number to Select Option: ";
            cin >> number;

            if (number == 1)
            {
                this->Print();
            }
            else if (number == 2)
            {
                this->Load_Data();
            }
            else if (number == 3)
            {
                int file_ID;
                cout << "Please enter the file ID : ";
                cin >> file_ID;
                file f1(file_ID);
                insert(f1);
            }
            else if (number == 4)
            {
                int file_Id;
                cout << "Enter File ID" << endl;
                cin >> file_Id;
                Delete_File(file_Id);
            }
            else if (number == 5)
            {
                int priority;
                string access;
                int file_Id, User_Id;
                cout << "Enter File ID : ";
                cin >> file_Id;
                cout << "Enter User ID : ";
                cin >> User_Id;
                cout << "Enter Priority :  ";
                cin >> priority;
                cout << "Enter type of access read or write : ";
                cin >> access;

                Request_Access(User_Id, access, priority, file_Id);
            }
            else if (number == 6)
            {
                int file_ID, User_ID;

                cout << "Enter File ID : ";
                cin >> file_ID;
                cout << "Enter User ID : ";
                cin >> User_ID;
                Release_File(file_ID, User_ID);
            }
            else if (number == 7)
            {
                cout << "Exit" << endl;
            }
            else
            {
                cout << "Wrong Entry " << endl;
            }

        }
    }
    friend class User_Information;
    friend class file;
};
// User info < class of priority queue>


int main()
{
    User<file> l;
    file f1(12);
    file f2(13);
    file f3(1);
    file f4(45);

    l.insert(f1);
    l.insert(f2);
    l.insert(f3);
    l.insert(f4);
    l.Load_Data();
    //l.Request_Access(2,"write",2, 1);
    //l.Request_Access(3,"read",3, 1);
  //  l.Request_Access(4,"read",4, 1);
   // l.Request_Access(1,"write",1, 1);*/
    l.menu();
    /* l.Release_File(1, 2);
     l.Delete_File(45);
     l.Print();*/
    system("pause");
    return 0;
}