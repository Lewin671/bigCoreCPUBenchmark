package com.example.bindbigcorecpu

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Process
import android.os.SystemClock
import android.util.Log
import android.widget.Button
import android.widget.TextView
import android.widget.Toast
import androidx.annotation.UiThread
import com.example.bindbigcorecpu.databinding.ActivityMainBinding
import kotlin.math.log

import kotlin.random.Random

class MainActivity : AppCompatActivity() {
    private val TAG = "MainActivity"

    private lateinit var binding: ActivityMainBinding
    private var logTextView: TextView? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        findViewById<Button>(R.id.startRacing).setOnClickListener {
            // Example of a call to a native method
            startSortingThreads(generateRandomList())
        }

        logTextView = findViewById(R.id.logTextView)

        log("应用启动成功")

    }

    /**
     * 生成200万个随机整数并返回一个List<Int>
     */
    private fun generateRandomList(): List<Int> {
        val list = mutableListOf<Int>()
        val random = Random(System.currentTimeMillis())
        for (i in 1..2000) {
            list.add(random.nextInt())
        }
        return list
    }

    /**
     * 使用插入排序对 List<Int> 进行排序
     * 时间复杂度：O(n^2)
     * 稳定性：稳定排序
     *
     * @param list 需要排序的列表
     * @return 排序后的新列表
     */
    private fun insertionSort(list: List<Int>): List<Int> {
        // 创建列表的可变副本以进行排序
        val sortedList = list.toMutableList()
        val n = sortedList.size

        for (i in 1 until n) {
            val key = sortedList[i]
            var j = i - 1

            // 将大于 key 的元素向右移动一位
            while (j >= 0 && sortedList[j] > key) {
                sortedList[j + 1] = sortedList[j]
                j--
            }

            // 插入 key
            sortedList[j + 1] = key
        }

        return sortedList
    }

    /**
     * 启动100个线程，每个线程对传入的列表进行排序，检测哪个线程先完成
     *
     * @param list 要排序的原始列表
     * @return 返回最先完成排序的线程编号（从1到100），如果所有线程都完成则返回-1
     */
    @UiThread
    private fun startSortingThreads(list: List<Int>) {
        // 使用 CountDownLatch 来等待所有线程完成
        logTextView?.text = ""

        for (i in 1..50) {
            val runnable = Runnable {
                if (i == 8) {
                    bringToBigCore()
                    Process.setThreadPriority(Process.THREAD_PRIORITY_URGENT_AUDIO)
                }
                // 创建列表的副本以避免多线程修改同一个列表
                val start = SystemClock.uptimeMillis()
                val listCopy = ArrayList(list)
                // 排序
                insertionSort(listCopy)
                log("线程 $i 完成排序, 耗时: ${SystemClock.uptimeMillis() - start}。")
            }

            Thread(runnable).apply {
                name = "SortingThread#$i"
                start()
            }
        }

    }

    private fun log(msg: String) {
        Log.i(TAG, msg)
        runOnUiThread {
            logTextView?.let {
                val newText = "${it.text}\n$msg"
                it.text = newText
            }
        }
    }

    private external fun bringToBigCore(): Boolean

    companion object {
        // Used to load the 'bindbigcorecpu' library on application startup.
        init {
            System.loadLibrary("bindbigcorecpu")
        }
    }
}