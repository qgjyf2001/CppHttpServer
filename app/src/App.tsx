import React,{useState} from 'react';
import { message,Button,Image ,Table} from 'antd';
import ProList from '@ant-design/pro-list';
import { DownloadOutlined,DeleteOutlined,FolderOutlined,FileOutlined} from '@ant-design/icons';
const App =function (){ 
  const [loading,setLoading]=useState(true);
  const currentDir=GetQueryString(window.location.search,"dir");
  const [dataSource,setDataSource]=useState([]);
  function GetQueryString(paraPart:string,name:string) {
    var reg = new RegExp("(^|&)" + name + "=([^&]*)(&|$)");
    var r = paraPart.substr(1).match(reg);
    if (r != null) return unescape(r[2]); return "";
}

  if (loading)
  {
    fetch('/api/dir?dir='+currentDir,{
      method:'GET'
    }).then((response)=>{
      return response.json();
    }).then((data)=>{
      for (var k of data)
        k.image=k.type===4?<FolderOutlined/>:<FileOutlined/>
      setLoading(false);
      setDataSource(data);
    })
  }
  return (
    
    <ProList<any>
    onRow={(record: any) => {
      return {
        onClick: () => {
          if (record.type===4)
          {
            window.location.href="/index.html?dir="+currentDir+record.name+"/";
          }
          if (record.type===8)
          {
            window.location.href="/file?file="+currentDir+record.name;
          }
        },
      };
    }}
    rowKey="name"
    headerTitle="文件列表"
    loading={loading}
    dataSource={dataSource}
    showActions="hover"
    showExtra="hover"
    metas={{
      title: {
        dataIndex: 'name',
      },
      avatar: {
        dataIndex: 'image',
      },
      actions: {
        render: (text, row) => [
          <Button type="primary" shape="round" icon={<DeleteOutlined/>} onClick={()=>{
              fetch('/api/remove?file='+currentDir+row.name).then((response)=>{
                return response.json();
              }).then((data)=>{
                if (data.status==="success")
                {
                  message.info("删除成功")
                  setTimeout(() => {
                    window.location.reload();
                  }, 2000);
                }
                else message.error("删除失败")
              })
          }}/>,
          <Button type="primary" shape="round" icon={<DownloadOutlined />} onClick={()=>{
            if (row.type===4)
            {
              window.location.href="/folder?folder="+currentDir+row.name+"/";
            }
            if (row.type===8)
            {
              window.location.href="/file?file="+currentDir+row.name;
            }
            }} />
        ],
        search: false,
      },
      }}
      
      />
);
  }
export default App;
