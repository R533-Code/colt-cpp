#ifndef HG_COLT_LIST
#define HG_COLT_LIST

#include "static_vector.h"

namespace clt
{
  template<typename T, size_t PER_NODE = 16, auto ALLOCATOR = mem::GlobalAllocatorDescription>
    requires meta::AllocatorScope<ALLOCATOR>
  class FlatList
  {
    /// @brief Doubly linked Node
    struct Node
    {
      /// @brief Owning pointer (to free) pointing to the next node
      Node* after = nullptr;
      /// @brief Pointer to the node before
      Node* before = nullptr;
      /// @brief The data
      StaticVector<T, PER_NODE> data;
    };

    /// @brief Creates an empty node
    /// @return Node allocated through the allocator
    constexpr Node* create_node() noexcept
    {
      return static_cast<Node*>(allocator.alloc(sizeof(Node)).ptr());
    }    

    /// @brief True if the allocator is global
    static constexpr bool is_global = meta::GlobalAllocator<ALLOCATOR>;
    /// @brief True if the allocator is local (we need a reference to it)
    static constexpr bool is_local = meta::LocalAllocator<ALLOCATOR>;

    /// @brief Type of the allocator
    using Allocator = mem::allocator_ref<ALLOCATOR>;

    /// @brief The allocator used for allocation/deallocation
    [[no_unique_address]]
    Allocator allocator;

    /// @brief Owning pointer to the head of the list, never null
    Node* head = create_node();
    /// @brief Pointer to the tail of the list
    Node* tail = head;
    /// @brief Pointer to the last active node of the list
    Node* last_active_node = head;
    /// @brief Count of active elements
    size_t count = 0;

    /// @brief Creates and appends a node to the end of the list
    constexpr void create_and_append_node() noexcept
    {
      auto before = tail; //copy pointer
      tail = create_node(); //set the tail to the new Node
      tail->before = before; //set the tail before to the old tail
      before->after = tail; //set the old tail's after to the new node
    }

    /// @brief Advances the last_active_node to the next node, appending a new node if needed
    constexpr void advance_active_node() noexcept
    {
      if (last_active_node == tail)
      {
        create_and_append_node();
        last_active_node = tail;
      }
      else
        last_active_node = last_active_node->after;
    }

  public:
    /// @brief Constructs an empty FlatList.
    /// This will always preallocate one Node.
    constexpr FlatList() noexcept requires is_global = default;

    template<meta::Allocator AllocT> requires is_local
    /// @brief Default constructor (when allocator is local)
    /// @param alloc Reference to the allocator to use
    constexpr FlatList(AllocT& alloc) noexcept
      : allocator(alloc) {}

    /// @brief Constructs an empty FlatList, reserving max('node_reserve_count', 1).
    /// Each Node can contain up to 'PER_NODE'.
    /// @param node_reserve_count The count of nodes to preallocate
    constexpr FlatList(size_t node_reserve_count) noexcept requires is_global
    {
      for (size_t i = 1; i < node_reserve_count; i++)
        create_and_append_node();
    }

    template<meta::Allocator AllocT> requires is_local
    /// @brief Constructs an empty FlatList, reserving max('node_reserve_count', 1).
    /// Each Node can contain up to 'PER_NODE'.
    /// @param node_reserve_count The count of nodes to preallocate
    constexpr FlatList(AllocT& alloc, size_t node_reserve_count) noexcept
      : allocator(alloc)
    {
      for (size_t i = 1; i < node_reserve_count; i++)
        create_and_append_node();
    }

    /// @brief Move constructor
    /// @param list The list whose content to steal
    constexpr FlatList(FlatList&& list) noexcept
      : allocator(list.allocator)
      , head(std::exchange(list.head, list.create_node()))
      , tail(std::exchange(list.tail, list.head))
      , last_active_node(std::exchange(list.last_active_node, list.head))
      , count(std::exchange(list.count, 0))
    {}

    /// @brief Clears the content of the list, without modifying capacity
    constexpr void clear() noexcept(std::is_nothrow_destructible_v<T>)
    {
      auto cpy = head;
      while (cpy != last_active_node)
      {
        head->data.clear();
        cpy = head->after;
      }
      last_active_node = head;
    }

    /// @brief Destructor
    ~FlatList() noexcept(std::is_nothrow_destructible_v<T>)
    {
      while (tail != head)
      {
        auto before = tail->before; //store the tail's preceding
        allocator.dealloc(mem::MemBlock{ tail, sizeof(Node) });
        tail = before;
      }
      head->data.clear();
      //Free the head node, which is always allocated
      allocator.dealloc(mem::MemBlock{ head, sizeof(Node) });
    }

    /// @brief Returns the count of the FlatListf
    /// @return The count of active objects in the list
    constexpr size_t size() const noexcept { return count; }

    /// @brief Check if the FlatList is empty.
    /// Equivalent to 'count() == 0'.
    /// @return True if the list is empty
    constexpr bool is_empty() const noexcept { return count == 0; }

    /// @brief Returns the object at index 'index' of the FlatList.
    /// @param index The index of the object
    /// @return The object at index 'index'
    constexpr meta::copy_trivial_t<const T&> operator[](size_t index) const noexcept
      COLT_PRE(index < this->size())
    {
      //Compiler should optimize this into a single instruction
      size_t div = index / PER_NODE;
      size_t rem = index % PER_NODE;

      auto head_c = head;
      while (div-- != 0)
        head_c = head_c->after;
      return head_c->data[rem];
    }
    COLT_POST()

    /// @brief Returns a reference to the object at index 'index' of the FlatList.
    /// @param index The index of the object
    /// @return The object at index 'index'
    constexpr T& operator[](size_t index) noexcept
      COLT_PRE(index < this->size())
    {
      //Compiler should optimize this into a single instruction
      size_t div = index / PER_NODE;
      size_t rem = index % PER_NODE;

      auto head_c = head;
      while (div-- != 0)
        head_c = head_c->after;
      return head_c->data[rem];
    }
    COLT_POST()

    /// @brief Returns the first item in the FlatList.
    /// @return The first item in the FlatList.
    constexpr meta::copy_trivial_t<const T&> front() const noexcept
      COLT_PRE(!this->is_empty())
      return head->data.front();
    COLT_POST()
    
    /// @brief Returns the first item in the FlatList.
    /// @return The first item in the FlatList.
    constexpr T& front() noexcept
      COLT_PRE(!this->is_empty())
      return head->data.front();
    COLT_POST()

    /// @brief Returns the last item in the FlatList.
    /// @return The last item in the FlatList.
    constexpr meta::copy_trivial_t<const T&> back() const noexcept
      COLT_PRE(!this->is_empty())
      return last_active_node->data.back();
    COLT_POST()
    
    /// @brief Returns the last item in the FlatList.
    /// @return The last item in the FlatList.
    constexpr T& back() noexcept
      COLT_PRE(!this->is_empty())
      return last_active_node->data.back();
    COLT_POST()

    template<typename... Args>
    /// @brief Pushes an item at the end of the FlatList
    void push_back(Args&&... args)
    {
      if (last_active_node->data.is_full())
        advance_active_node();
      last_active_node->data.push_back(std::forward<Args>(args)...);
      ++count;
    }

    /// @brief Pops an item from the back of the FlatList.
    constexpr void pop_back()
      noexcept(std::is_nothrow_destructible_v<T>)
      COLT_PRE(!this->is_empty())
    {
      --count;
      if (last_active_node->data.size() == 0)
        last_active_node = last_active_node->before;
      last_active_node->data.pop_back();
    }
    COLT_POST()

  private:
    
    template<typename Node_t>
    class Iterator
    {
      size_t node_index;
      Node_t* current_node;

    public:
      constexpr Iterator(Node_t* node, size_t index = 0) noexcept
        : node_index(index), current_node(node) {
        assert(index < PER_NODE);
      }

      constexpr Iterator& operator++() noexcept
      {
        if (node_index + 1 == PER_NODE)
        {
          current_node = current_node->after;
          node_index = 0;
        }
        else
          ++node_index;
        return *this;
      }

      constexpr Iterator operator++(int) noexcept
      {
        auto copy = *this;
        ++(*this);
        return copy;
      }

      constexpr Iterator& operator--() noexcept
      {
        if (node_index - 1 == 0)
        {
          current_node = current_node->before;
          node_index = PER_NODE - 1;
        }
        else
          --node_index;
        return *this;
      }

      constexpr Iterator operator--(int) noexcept
      {
        auto copy = *this;
        --(*this);
        return copy;
      }

      constexpr T* operator->() noexcept { return current_node->data.data() + node_index; }
      constexpr meta::copy_trivial_t<const T&> operator*() noexcept { return current_node->data[node_index]; }

      friend constexpr bool operator==(const Iterator& i1, const Iterator& i2) noexcept
      {
        return i1.node_index == i2.node_index && i1.current_node == i2.current_node;
      }

      friend constexpr bool operator!=(const Iterator& i1, const Iterator& i2) noexcept
      {
        return !(i1 == i2);
      }
    };

  public:
    /// @brief Returns an iterator to the beginning of the list
    /// @return Iterator to the beginning of the list
    constexpr Iterator<Node> begin() noexcept { return { head, 0 }; }
    /// @brief Returns an iterator to the beginning of the list
    /// @return Iterator to the beginning of the list
    constexpr Iterator<const Node> begin() const noexcept { return { head, 0 }; }

    /// @brief Returns an iterator to the end of the list
    /// @return Iterator to the end of the list
    constexpr Iterator<Node> end() noexcept
    {
      if (last_active_node->data.is_full())
        return { last_active_node->after, 0 };
      return { last_active_node, last_active_node->data.size() };
    }
    /// @brief Returns an iterator to the end of the list
    /// @return Iterator to the end of the list
    constexpr Iterator<const Node> end() const noexcept
    {
      if (last_active_node->data.is_full())
        return { last_active_node->after, 0 };
      return { last_active_node, last_active_node->data.size() };
    }
  };

  template<meta::Parsable T, size_t PER_NODE, auto ALLOCATOR> requires meta::AllocatorScope<ALLOCATOR>
  /// @brief Overload for Vector
  struct str::parser<FlatList<T, PER_NODE, ALLOCATOR>>
    : str::Recommended<str::recommended_size<T>() * PER_NODE>
  {
    constexpr ParseResult operator()(maybe_out<FlatList<T, PER_NODE, ALLOCATOR>> result, StringView to_parse) const noexcept
    {
      if (to_parse.size() < 2 || to_parse.front() != '[' || to_parse.back() != ']')
        return ParseResult{ to_parse.end(), ParseErrorCode::INVALID_FMT };
      auto end = to_parse.end();
      //Pop '[' and ']'
      to_parse.pop_front(); to_parse.pop_back();

      //Construct the Vector
      result.construct();

      if (to_parse.is_empty())
        return ParseResult{ end, ParseErrorCode::SUCCESS };

      u64 offset = 0;
      u64 find_i = to_parse.find(',', offset);
      do
      {
        auto obj = to_parse.substr(offset, find_i - offset);
        obj.strip();

        uninit<T> try_obj;
        auto [ptr, err] = clt::str::parser<T>{}(try_obj, obj);
        if (err != ParseErrorCode::SUCCESS)
        {
          //Destroy the object as it was constructed...
          result.underlying().destruct();
          return ParseResult{ obj.end(), err };
        }

        ON_SCOPE_EXIT{
          //Destroy the object as it was constructed...
          try_obj.destruct();
        };

        if (ptr != obj.end())
        {
          //Destroy the object as it was constructed...
          result.underlying().destruct();
          return ParseResult{ ptr, ParseErrorCode::INVALID_FMT };
        }

        result.data().push_back(std::move(try_obj.data()));
        //After ','
        offset = find_i + 1;
        find_i = to_parse.find(',', offset);
        //Due to overflow, when find_i is npos, find_i + 1 == 0.
        if (offset == 0)
          break;
      } while (true);

      return ParseResult{ end, ParseErrorCode::SUCCESS };
    }
  };
}

template<typename T, size_t PER_NODE, auto ALLOCATOR>
  requires fmt::is_formattable<T>::value
struct fmt::formatter<clt::FlatList<T, PER_NODE, ALLOCATOR>>
{
  bool human_readable = false;

  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    auto it = ctx.begin();
    auto end = ctx.end();
    if (it == end)
      return it;
    if (*it == 'h')
    {
      ++it;
      human_readable = true;
    }
    assert_true("Possible format for FlatList are: {} or {:h}!", *it == '}');
    return it;
  }

  template<typename FormatContext>
  auto format(const clt::FlatList<T, PER_NODE, ALLOCATOR>& vec, FormatContext& ctx)
  {
    auto fmt_to = ctx.out();
    
    auto begin = vec.begin();    
    auto end = vec.end();
    if (human_readable)
    {
      if (begin != end)
      {
        fmt_to = fmt::format_to(fmt_to, "{}", *begin);
        ++begin;
      }
      if (vec.size() > 1)
      {
        for (size_t i = 1; i < vec.size() - 1; i++)
        {
          fmt_to = fmt::format_to(fmt_to, ", {}", *begin);
          ++begin;
        }
        fmt_to = fmt::format_to(fmt_to, " and {}", *begin);
      }
      return fmt_to;
    }
    else
    {
      fmt_to = fmt::format_to(fmt_to, "[");
      if (begin != end)
      {
        fmt_to = fmt::format_to(fmt_to, "{}", *begin);
        ++begin;
      }
      for (size_t i = 1; i < vec.size(); i++)
      {
        fmt_to = fmt::format_to(fmt_to, ", {}", *begin);
        ++begin;
      }
      return fmt::format_to(fmt_to, "]");
    }
  }
};

#endif //!HG_COLT_LIST