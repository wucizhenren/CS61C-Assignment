import sys
import re

from pyspark import SparkContext,SparkConf

def flatMapFunc(document):
    """
    document[0] is the document ID (distinct for each document)
    document[1] is a string of all text in that document

    You will need to modify this code.
    """
    documentID = document[0]
    words = re.findall(r"\w+", document[1])
    # 第一步：生成 (word, index)
    tuples = [(word, i + 1) for i, word in enumerate(words)]

    # 第二步：分组
    resultDict = {}

    for word, index in tuples:
      if word not in resultDict:
         resultDict[word] = [index]
      else:
          resultDict[word].append(index)

    # 第三步：转成 tuple
    result = [(word, documentID,*indexes) for word, indexes in resultDict.items()]

    return result

def mapFunc(arg):
    """
    You may need to modify this code.
    """
    return (arg, 1)

def reduceFunc(arg1, arg2):
    """
    You may need to modify this code.
    """
    return arg1+arg2

def createIndices(file_name, output="spark-wc-out-createIndices"):
    sc = SparkContext("local[8]", "CreateIndices", conf=SparkConf().set("spark.hadoop.validateOutputSpecs", "false"))
    file = sc.sequenceFile(file_name)

    indices = file.flatMap(flatMapFunc)

    indices.coalesce(1).saveAsTextFile(output)

""" Do not worry about this """
if __name__ == "__main__":
    argv = sys.argv
    if len(argv) == 2:
        createIndices(argv[1])
    else:
        createIndices(argv[1], argv[2])
