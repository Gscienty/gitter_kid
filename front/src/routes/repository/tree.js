import React from 'react';
import ProjectTemplate from './_project';
import { Table, Card } from 'antd';
import { connect } from 'react-redux';
import  FolderIcon from 'react-icons/lib/io/folder';
import  FileIcon from 'react-icons/lib/go/file-code';

class Page extends ProjectTemplate {
    state = { initialized: false }

    freshTreeItems () {
        this.props.dispatch({
            type: 'tree/getTree',
            payload: {
                repositoriesName: this.props.match.params.repositoriesName,
                repositoryName: this.props.match.params.repositoryName,
                branchName: this.props.match.params.branchName,
            },
            after: () => this.setState({ initialized: true })
        });
    }

    componentWillMount () { this.freshTreeItems (); }
    componentWillReceiveProps() { this.setState ({ initialized: false }, () => this.freshTreeItems ()); }

    waitingRender () {
        return <span>waiting...</span>;
    }

    treeItemOnClick (record) {
        this.setState({ initialized: false }, () => this.props.dispatch({
            type: 'tree/enterSubTree',
            payload: record,
            after: () => this.setState({ initialized: true })
        }));
    }

    renderPath() {
        let treePath = this.props.treePath;
        if (treePath.endsWith('/')) {
            treePath = treePath.substring (0, treePath.length - 1);
        }
        let pathItems = treePath.split('/');
        pathItems[0] = { name: '', path: '' };
        for (let i = 1; i < pathItems.length; i++) {
            pathItems[i] = { name: pathItems[i], path: pathItems[i - 1].path + '/' + pathItems[i] };
        }

        pathItems[0].name = this.props.match.params.repositoryName;
        pathItems[0].path = '/';

        return pathItems.map((item, index) => <span style={{ fontSize: 21 }} key={ index }>
            { index === 0 ? '' : '/' }
            <span style={{ margin: '0 4px' }}>
                {
                    index === pathItems.length - 1 && index !== 0
                    ? item.name
                    : <a
                        onClick={() => this.treeItemOnClick ({ path: item.path, type: 'Tree' }) }>
                        { item.name }
                    </a>
                }
            </span>
        </span>);
    }

    treeRender () {
        return <div>
            <Card
                title={ this.renderPath () }
                bodyStyle={{ padding: 0 }}>
                <Table
                    columns={[{
                        key: 'fileName',
                        render: (text, record) => <a onClick={ () => this.treeItemOnClick (record) }>
                            <span style={{ fontSize: 18 }}>{ record.type === 'Blob' ? <FileIcon /> : <FolderIcon /> }</span>
                            <span style={{ marginLeft: 8 }}>{ record.name }</span>
                        </a>
                    }]}
                    dataSource={ this.props.treeItems.map(e => ({ ...e, key: e.name })) }
                    showHeader={ false }
                    pagination={ false }
                    size="small"
                />
            </Card>
        </div>;
    }

    unit() {
        return this.state.initialized ? this.treeRender() : this.waitingRender();
    }
}

export default connect(
    ({ tree, branches }) => ({
        entryUnit: 'repositories',
        projectEntryUnit: 'code',
        treeItems: tree.items,
        treePath: tree.path,
        branches: branches.branches
    })
)(Page);