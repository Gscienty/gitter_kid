import React from 'react';
import ProjectTemplate from './_project';
import { Table, Icon, Card, Dropdown, Button, Menu } from 'antd';
import { connect } from 'react-redux';

class Page extends ProjectTemplate {
    state = { path: '', prefixURI: '' }

    entryTreeInitialize() {
        let pathItems = window.location.hash.substring(1).split('/tree');
        let path = '/';
        if (pathItems.length === 2) {
            path = pathItems[1];
        }

        if (path[path.length - 1] !== '/') {
            path = path + '/';
        }

        if (path !== this.state.path) {
            this.setState({ path }, () => this.freshTree(path, this.props.match.params.branchName));
        }
    }

    componentDidMount() {
        this.setState({ prefixURI: this.props.match.url.split('/tree')[0] });
        this.entryTreeInitialize();
        this.props.dispatch({
            type: 'branches/getBranches',
            payload: {
                repositoriesName: this.props.match.params.repositoriesName,
                repositoryName: this.props.match.params.repositoryName
            }
        });
    }

    componentWillReceiveProps(props) {
        this.entryTreeInitialize();
    }

    freshTree(path, branchName) {
        this.props.history.push(this.state.prefixURI  + '/tree' + path);
        this.props.dispatch({
            type: 'tree/getTree',
            payload: {
                repositoriesName: this.props.match.params.repositoriesName,
                repositoryName: this.props.match.params.repositoryName,
                branchName,
                path
            }
        });
    }

    treeItemOnClick(record) {
        if (record.type === 'Tree') {
            this.freshTree(this.state.path + record.name + '/', this.props.match.params.branchName);
        }
    }

    renderPath() {
        let pathItems = this.state.path.substring(0, this.state.path.length - 1).split('/');
        pathItems[0] = {
            name: '',
            path: ''
        }
        for (let i = 1; i < pathItems.length; i++) {
            pathItems[i] = {
                name: pathItems[i],
                path: pathItems[i - 1].path + '/' + pathItems[i]
            };
        }

        pathItems[0].name = this.props.match.params.repositoryName;
        pathItems[0].path = '/';

        return pathItems.map((item, index) => <span style={{ fontSize: 21 }} key={ index }>
            { index === 0 ? '' : '/' }
            <a onClick={() => this.freshTree(item.path, this.props.match.params.branchName)}
            style={{ margin: '0 4px' }}>{ item.name }</a>
        </span>);
    }

    columns = [
        {
            key: 'fileName',
            render: (text, record) => <a onClick={ () => this.treeItemOnClick(record) }>
                <span>{ record.type === 'Blob' ? <Icon type="file-text" /> : <Icon type="folder" /> }</span>
                <span style={{ marginLeft: 8 }}>{ record.name }</span>
            </a>
        }
    ]

    branchesSelectRender() {
        return <Menu onClick={ e => this.freshTree('/', e.key) }>
            { this.props.branches.map((branch, index) => <Menu.Item
                key={ branch }
                style={{ textAlign: 'center' }}>{ branch }</Menu.Item>) }
        </Menu>;
    }

    unit() {    
        return <Card
            title={ <span>{ this.renderPath() }</span> }
            extra={ <div>
                <Dropdown overlay={ this.branchesSelectRender() } trigger={['click']}>
                    <Button>{ this.props.match.params.branchName } <Icon type="down" /></Button>
                </Dropdown>
            </div> }
            bodyStyle={{ padding: 0 }}>
            <Table
                columns={ this.columns }
                dataSource={ this.props.tree.content.map(e => ({ ...e, key: e.name })) }
                showHeader={ false }
                pagination={ false }
            />
        </Card>;
    }
}

export default connect(
    ({ tree, branches }) => ({
        entryUnit: 'repositories',
        projectEntryUnit: 'code',
        tree,
        branches: branches.branches
    })
)(Page);