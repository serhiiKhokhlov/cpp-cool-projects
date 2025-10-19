#pragma once

class ResourcePool {
protected:
    int m_free_cnt{};
    int m_total_cnt{};
public:
    ResourcePool(int free_cnt) : m_free_cnt(free_cnt), m_total_cnt{free_cnt} {}
    virtual ~ResourcePool() {}

    virtual bool acquire(int rc_cnt) {
        if (m_free_cnt >= rc_cnt) {
            m_free_cnt -= rc_cnt;
            return true;
        }
        return false;
    }

	int getFreeCount() const {
		return m_free_cnt;
	}

    void setNewTotalCount(int new_total_cnt) {
        if (new_total_cnt >= (m_total_cnt - m_free_cnt)) {
            m_free_cnt = new_total_cnt - (m_total_cnt - m_free_cnt);
            m_total_cnt = new_total_cnt;
        }
    }

    int getTotalCount() const {
        return m_total_cnt;
    }

    int getBusyCount() const {
        return m_total_cnt - m_free_cnt;
    }

    virtual void release(int rc_cnt) {
        m_free_cnt += rc_cnt;
    }
};