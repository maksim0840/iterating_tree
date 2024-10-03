#include <memory> // shared_ptr
#include <iterator> // iterator_traits, iterator
#include <iostream>
#include <utility> // pair

template<class K, class V>  
class SearchingTree { 
    
private:
    struct TreeNode {
        K key_;
        V value_;
        std::shared_ptr<TreeNode> left; // less
        std::shared_ptr<TreeNode> right; // greater

        // Constructor (for make_shared ptr command)
        TreeNode(const K& key, const V& value, std::shared_ptr<TreeNode> left, std::shared_ptr<TreeNode> right)
            : key_(key), value_(value), left(std::move(left)), right(std::move(right)) {}

        // Copy constructor
        TreeNode(const TreeNode& node) 
            : key_(node.key_), value_(node.value_), left(node.left), right(node.right) {}
    };

    std::shared_ptr<TreeNode> top = nullptr;

public: 
    // Iterator
    template <class iterator_type> // const or not const
    class TreeIterator {
        friend class SearchingTree; // for private constructor
    public:
        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = iterator_type; 
        using pointer = std::shared_ptr<iterator_type>;
        using reference = iterator_type&;

        std::shared_ptr<iterator_type> node_; // iterator node
        std::shared_ptr<iterator_type> top_; // top of the tree
    private: // can be created only in SearchingTree (need to be linked to object)
        // Constructor
        TreeIterator(std::shared_ptr<iterator_type> node, std::shared_ptr<iterator_type> top) 
            : node_(node), top_(top) {}
    public:
        // Copy constructor
        TreeIterator(const TreeIterator& iter) : node_(iter.node_), top_(iter.top_) {}

        // Operators
        bool operator!=(TreeIterator const& other) const {
            return node_ != other.node_;
        }

        bool operator==(TreeIterator const& other) const {
            return node_ == other.node_;
        }

        std::pair<K, V> operator*() const {
            return std::pair<K, V>(node_->key_, node_->value_); // for output
        }

        TreeIterator& operator++() { // prefix ++

            std::shared_ptr<iterator_type> last_parent_before_turning_left = nullptr;
            std::shared_ptr<iterator_type> cur_node = top_;

            // Find iterator's node in the tree
            while (cur_node != nullptr) {

                if (node_->key_ < cur_node->key_) {
                    last_parent_before_turning_left = cur_node;
                    cur_node = cur_node->left;
                }
                else if (node_->key_ > cur_node->key_) {
                    cur_node = cur_node->right;
                }
                else {
                    if (cur_node->right != nullptr) {
                        cur_node = cur_node->right;
                        // Find the smallest num, that bigger than cur_node
                        while (cur_node->left != nullptr) {
                            cur_node = cur_node->left;
                        }
                        node_ = cur_node;
                    }
                    else {
                        node_ = last_parent_before_turning_left;
                    }
                    break;
                }

            }
            return *this;
        }        

    };

    // Iterator methods
    using iterator = TreeIterator<TreeNode>;
    using const_iterator = TreeIterator<const TreeNode>;

    iterator begin() {
        std::shared_ptr<TreeNode> cur_node = top;
        std::shared_ptr<TreeNode> last_parent = nullptr;

        while (cur_node != nullptr) {
            last_parent = cur_node;
            cur_node = cur_node->left;
        }

        TreeIterator<TreeNode> iter(last_parent, top);
        return iter;
    }

    iterator end() {
        TreeIterator<TreeNode> iter(nullptr, nullptr);
        return iter;
    }

    const_iterator begin() const {
        return const_iterator(begin());
    }

    const_iterator end() const {
        return const_iterator(end());
    }


    // Tree methods
    void insert(const K &key, const V &value) {
        std::shared_ptr<TreeNode> cur_node = top;

        if (cur_node == nullptr) {
            top = std::make_shared<TreeNode>(key, value, nullptr, nullptr);
            return;
        }

        while(cur_node != nullptr) {
            if (key < cur_node->key_) {
                if (cur_node->left == nullptr) {
                    cur_node->left = std::make_shared<TreeNode>(key, value, nullptr, nullptr);
                    break;
                }
                else {
                    cur_node = cur_node->left;
                }
            }
            else if (key > cur_node->key_) {
                if (cur_node->right == nullptr) {
                    cur_node->right = std::make_shared<TreeNode>(key, value, nullptr, nullptr);
                    break;
                }
                else {
                    cur_node = cur_node->right;
                }
            }
            else {
                cur_node->value_ = value;
                break;
            }
        }
    } 

    void erase(const K &key) {
        std::shared_ptr<TreeNode> cur_node = top;
        std::shared_ptr<TreeNode> parent = nullptr;
        bool turn_left = false;

        while (cur_node != nullptr) {
            if (key < cur_node->key_) {
                parent = cur_node;
                cur_node = cur_node->left;
                turn_left = true;
            }
            else if (key > cur_node->key_) {
                parent = cur_node;
                cur_node = cur_node->right;
                turn_left = false;
            }
            else {
                break;
            }
        }

        if (cur_node == nullptr) {
            return;
        }

        // No childs
        if (cur_node->left == nullptr && cur_node->right == nullptr) {
            if (parent == nullptr) {
                top = nullptr;
                return;
            }
            else if (turn_left) {
                parent->left = nullptr;
            }
            else {
                parent->right = nullptr;
            }
        }
        // One child
        else if (cur_node->left == nullptr || cur_node->right == nullptr) {
            if (turn_left) {
                parent->left = (cur_node->left == nullptr) ? cur_node->right : cur_node->left;
            }
            else {
                parent->left = (cur_node->left == nullptr) ? cur_node->right : cur_node->left;
            }
        }
        // Both childs
        else {
            std::shared_ptr<TreeNode> max_left = cur_node->left;
            std::shared_ptr<TreeNode> max_left_parent = cur_node;
            bool turn_right = false;

            while(max_left->right != nullptr) {
                turn_right = true;
                max_left_parent = max_left;
                max_left = max_left->right;
            }

            // Put max_left to the deleted place
            cur_node->key_ = max_left->key_;
            cur_node->value_ = max_left->value_;

            // Save childs
            if (turn_right) {
                max_left_parent->right = max_left->left;
            }
            else {
                cur_node->left = max_left->left;
            }
        }
    } 

    iterator find(const K &key) {
        std::shared_ptr<TreeNode> cur_node = top;

        while(cur_node != nullptr) {
            if (key < cur_node->key_) {
                cur_node = cur_node->left;
            }
            else if (key > cur_node->key_) {
                cur_node = cur_node->right;
            }
            else {
                TreeIterator<TreeNode> iter(cur_node, top);
                return iter;
            }
        }

        return nullptr;
    }

    class Subrange {
    private:
        iterator start_;
        iterator finish_;
    public:
        Subrange(iterator start, iterator finish) : start_(start), finish_(finish) {}

        iterator begin() {
            return start_;
        }

        iterator end() {
            return finish_;
        }
    };

    // Range realisation "tree.range(a, b)"
    Subrange range(const int a, const int b) {

        // Loop iterators
        const iterator begin_iter = begin();
        const iterator end_iter = end();
        // Class iterators
        iterator start = end_iter;
        iterator finish = end_iter;
        if (a >= b) {
            Subrange sub = Subrange(start, finish);
            return sub;
        }

        bool start_was_found = false;

        for (iterator iter = begin_iter; iter != end_iter; ++iter) {

            if (!start_was_found && (*iter).first >= a && (*iter).first < b) {
                start = iter;
                start_was_found = true;
            }
            if ((*iter).first >= b) {
                finish = iter;
                break;
            }
        }

        if (start == end_iter) {
            finish = end_iter;
        }

        Subrange sub = Subrange(start, finish);
        return sub;
    }

};
