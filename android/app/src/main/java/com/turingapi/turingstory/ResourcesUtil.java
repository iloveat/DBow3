package com.turingapi.turingstory;

import java.util.HashMap;

/**
 * Created by brycezou on 8/10/18.
 */

public class ResourcesUtil {

    private static HashMap<Integer, String> mIdx2BookIdMap = null;

    public static HashMap getIdx2BookIdMap() {
        if(mIdx2BookIdMap == null) {
            mIdx2BookIdMap = new HashMap<>();
            setIdx2BookIdMap();
        }
        return mIdx2BookIdMap;
    }

    private static void setIdx2BookIdMap() {
        mIdx2BookIdMap.put(-1, "不认识的书");

        mIdx2BookIdMap.put(0, "树林里的小松鼠罗宾");
        mIdx2BookIdMap.put(1, "树林里的小松鼠罗宾");
        mIdx2BookIdMap.put(2, "树林里的小松鼠罗宾");
        mIdx2BookIdMap.put(3, "树林里的小松鼠罗宾");
        mIdx2BookIdMap.put(4, "树林里的小松鼠罗宾");

        mIdx2BookIdMap.put(5, "怕黑的小北极熊");
        mIdx2BookIdMap.put(6, "怕黑的小北极熊");
        mIdx2BookIdMap.put(7, "怕黑的小北极熊");
        mIdx2BookIdMap.put(8, "怕黑的小北极熊");
        mIdx2BookIdMap.put(9, "怕黑的小北极熊");

        mIdx2BookIdMap.put(10, "小熊猫找朋友");
        mIdx2BookIdMap.put(11, "小熊猫找朋友");
        mIdx2BookIdMap.put(12, "小熊猫找朋友");
        mIdx2BookIdMap.put(13, "小熊猫找朋友");
        mIdx2BookIdMap.put(14, "小熊猫找朋友");

        mIdx2BookIdMap.put(15, "舒科特和秘密魔法");
        mIdx2BookIdMap.put(16, "舒科特和秘密魔法");
        mIdx2BookIdMap.put(17, "舒科特和秘密魔法");
        mIdx2BookIdMap.put(18, "舒科特和秘密魔法");
        mIdx2BookIdMap.put(19, "舒科特和秘密魔法");

        mIdx2BookIdMap.put(20, "克洛蒂尔德的夏令营");
        mIdx2BookIdMap.put(21, "克洛蒂尔德的夏令营");
        mIdx2BookIdMap.put(22, "克洛蒂尔德的夏令营");
        mIdx2BookIdMap.put(23, "克洛蒂尔德的夏令营");
        mIdx2BookIdMap.put(24, "克洛蒂尔德的夏令营");
    }

}
